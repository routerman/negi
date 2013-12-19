--
-- PostgreSQL database dump
--

SET statement_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SET check_function_bodies = false;
SET client_min_messages = warning;

--
-- Name: plpgsql; Type: EXTENSION; Schema: -; Owner:
--

CREATE EXTENSION IF NOT EXISTS plpgsql WITH SCHEMA pg_catalog;


--
-- Name: EXTENSION plpgsql; Type: COMMENT; Schema: -; Owner:
--

COMMENT ON EXTENSION plpgsql IS 'PL/pgSQL procedural language';


SET search_path = public, pg_catalog;

--
-- Name: config_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE config_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.config_id_seq OWNER TO postgres;

SET default_tablespace = '';

SET default_with_oids = false;

--
-- Name: config; Type: TABLE; Schema: public; Owner: postgres; Tablespace:
--

CREATE TABLE config (
    name text,
    value text,
    id integer DEFAULT nextval('config_id_seq'::regclass) NOT NULL
);


ALTER TABLE public.config OWNER TO postgres;

--
-- Name: error_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE error_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.error_id_seq OWNER TO postgres;

--
-- Name: error; Type: TABLE; Schema: public; Owner: postgres; Tablespace:
--

CREATE TABLE error (
    id integer DEFAULT nextval('error_id_seq'::regclass) NOT NULL,
    error text,
    "timestamp" timestamp without time zone DEFAULT now()
);


ALTER TABLE public.error OWNER TO postgres;

--
-- Name: regexp_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE regexp_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.regexp_id_seq OWNER TO postgres;

--
-- Name: rule; Type: TABLE; Schema: public; Owner: postgres; Tablespace:
--

CREATE TABLE rule (
    id integer DEFAULT nextval('regexp_id_seq'::regclass) NOT NULL,
    owner text,
    expire_date date DEFAULT now(),
    src_ip text,
    src_netmask text,
    src_port integer,
    dst_ip text,
    dst_netmask text,
    dst_port integer,
    "timestamp" timestamp without time zone DEFAULT now(),
    regexp text,
    prefilter_pattern text,
    prefilter_offset integer,
    prefilter_depth integer,
    save_flag integer
);


ALTER TABLE public.rule OWNER TO postgres;

--
-- Name: save_data_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE save_data_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
    CYCLE;


ALTER TABLE public.save_data_id_seq OWNER TO postgres;

--
-- Name: save_packet_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE save_packet_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
    CYCLE;


ALTER TABLE public.save_packet_id_seq OWNER TO postgres;

--
-- Name: save_packet; Type: TABLE; Schema: public; Owner: postgres; Tablespace:
--

CREATE TABLE save_packet (
    "timestamp" timestamp without time zone,
    src_ip text,
    dst_ip text,
    src_port integer,
    dst_port integer,
    packet_size integer,
    packet_size_org integer,
    content_size integer,
    error text,
    content bytea,
    id integer DEFAULT nextval('save_packet_id_seq'::regclass) NOT NULL,
    protocol integer,
    flag text
);


ALTER TABLE public.save_packet OWNER TO postgres;

--
-- Name: save_result_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE save_result_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
    CYCLE;


ALTER TABLE public.save_result_id_seq OWNER TO postgres;

--
-- Name: save_result; Type: TABLE; Schema: public; Owner: postgres; Tablespace:
--

CREATE TABLE save_result (
    id integer DEFAULT nextval('save_result_id_seq'::regclass) NOT NULL,
    stream_id integer,
    rule_id integer,
    pattern text,
    pattern_len integer,
    place integer,
    result bytea,
    "timestamp" timestamp without time zone,
    src_ip text,
    dst_ip text,
    src_port integer,
    dst_port integer
);


ALTER TABLE public.save_result OWNER TO postgres;

--
-- Name: save_stream; Type: TABLE; Schema: public; Owner: postgres; Tablespace:
--

CREATE TABLE save_stream (
    id integer DEFAULT nextval('save_data_id_seq'::regclass) NOT NULL,
    "timestamp" timestamp without time zone,
    src_ip text,
    dst_ip text,
    src_port integer,
    dst_port integer,
    protcol integer,
    direction integer,
    truncate integer,
    regexp_id text,
    stream bytea,
    st_size integer,
    st_size_org integer,
    l5prot integer,
    match_str bytea,
    http_size text,
    http_header_size text,
    http_chunked text,
    http_compress text,
    error text,
    stream_org bytea,
    srim_version text,
    hit_count integer,
    after_ipfilter integer DEFAULT (-1),
    after_prefilter integer DEFAULT (-1),
    prefilter_log text,
    l7_error integer,
    l7prot integer
);


ALTER TABLE public.save_stream OWNER TO postgres;


------------->routerman start

--
-- Name: record; Type: TABLE; Schema: public; Owner: postgres; Tablespace:
--

CREATE TABLE record (
    dst_ip text,
	 host text
);

ALTER TABLE public.record OWNER TO postgres;

--
-- Name: url_action_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE url_action_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
    CYCLE;


ALTER TABLE public.url_action_id_seq OWNER TO postgres;

--
-- Name: url_action; Type: TABLE; Schema: public; Owner: postgres; Tablespace:
--

CREATE TABLE url_action (
    id integer DEFAULT nextval('url_action_id_seq'::regclass) NOT NULL,
    host text,
    service_type text,
    method text,
	 url text,
	 referer text,	
    action text,
	 data text
);

ALTER TABLE public.url_action OWNER TO postgres;

--
-- Name: action_log_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE action_log_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
    CYCLE;


ALTER TABLE public.action_log_id_seq OWNER TO postgres;

--
-- Name: action_log; Type: TABLE; Schema: public; Owner: postgres; Tablespace:
--

CREATE TABLE action_log (
    id integer DEFAULT nextval('action_log_id_seq'::regclass) NOT NULL,
    "timestamp" timestamp without time zone,
	 src_ip text,
	 host text,
	 service_type text,
	 action text,
	 url text,
	 title text,
	 object text,
	 access_time integer
);

ALTER TABLE public.action_log OWNER TO postgres;


--
--Name: user_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE user_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
    CYCLE;


ALTER TABLE public.user_id_seq OWNER TO postgres;

--
-- Name: action_count; Type: TABLE; Schema: public; Owner: postgres; Tablespace:
--

CREATE TABLE action_count(
    id integer DEFAULT nextval('user_id_seq'::regclass) NOT NULL,
    src_ip text,
    host text,
	 access_day integer,
	 access_time_day integer,
    access_month integer,
	 access_time_month integer,
    cart integer,
    buy integer,
    class text,
	 train_flag integer
);

ALTER TABLE public.action_count OWNER TO postgres;


-------------->routerman end

--
-- Name: update_check; Type: TABLE; Schema: public; Owner: postgres; Tablespace:
--

CREATE TABLE update_check (
    value text,
    updated integer
);


ALTER TABLE public.update_check OWNER TO postgres;

--
-- Name: config_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace:
--

ALTER TABLE ONLY config
    ADD CONSTRAINT config_pkey PRIMARY KEY (id);


--
-- Name: regexp_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace:
--

ALTER TABLE ONLY rule
    ADD CONSTRAINT regexp_pkey PRIMARY KEY (id);


--
-- Name: save_data_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace:
--

ALTER TABLE ONLY save_stream
    ADD CONSTRAINT save_data_pkey PRIMARY KEY (id);


--
-- Name: public; Type: ACL; Schema: -; Owner: postgres
--

REVOKE ALL ON SCHEMA public FROM PUBLIC;
REVOKE ALL ON SCHEMA public FROM postgres;
GRANT ALL ON SCHEMA public TO postgres;
GRANT ALL ON SCHEMA public TO PUBLIC;



----------------------->routerman start

--
-- Data for Name: rule; Type: TABLE DATA; Schema: public; 
--

COPY rule (id, owner, expire_date, src_ip, src_netmask, src_port, dst_ip, dst_netmask, dst_port, "timestamp", regexp, prefilter_pattern, prefilter_offset, prefilter_depth, save_flag) FROM stdin;
101	routerman	2012-01-11	\N	\N	\N	\N	\N	\N	2011-01-11 14:40:36.136482	\N	GET 	0	0	1
102	routerman	2012-09-11	\N	\N	\N	\N	\N	\N	2011-09-11 21:36:39.451221	\N	POST 	0	0	1
103	routerman	2012-09-11	\N	\N	\N	\N	\N	\N	2011-09-11 21:36:39.451221	\N	HOST:	0	0	1
104	routerman	2012-09-11	\N	\N	\N	\N	\N	\N	2011-09-11 21:36:39.451221	\N	Cookie:	0	0	1
105	routerman	2012-09-11	\N	\N	\N	\N	\N	\N	2011-09-11 21:36:39.451221	\N	Referer:	0	0	1
\.

--
-- Data for Name: dns; Type: TABLE DATA; Schema: public; 
--

COPY record (dst_ip, host) FROM stdin;
54.240.248.0	www.amazon.co.jp
202.72.50.10	www.rakuten.co.jp
202.32.114.47	www.nitori-net.jp
210.129.151.129	kakaku.com
202.247.10.161	www.takashimaya.co.jp
69.171.229.25	www.facebook.com
203.216.231.189	www.yahoo.co.jp
\.

--
-- Data for Name: rule; Type: TABLE DATA; Schema: public; 
--

COPY url_action (id, host, method, url, referer, action, data) FROM stdin;
1	www.nitori-net.jp	POST	/shop/cart/cart.aspx	\N	cart	\N
2	www.amazon.jp	POST	/gp/product/handle-buy-box/ref	\N	cart	\N
3	www.amazon.jp	POST	'/gp/huc/csrf-add.html/ref'	\N	cart	\N
\.
--
-- Data for Name: rule; Type: TABLE DATA; Schema: public; 
--

COPY action_count ( id, src_ip, host, access_day, access_time_day, access_month, access_time_month, cart, buy, class, train_flag) FROM stdin;
1	\N	\N	50	\N	90	\N	50	10	Good	1
2	\N	\N	60	\N	90	\N	50	5	Good	1
3	\N	\N	90	\N	90	\N	50	3	Good	1
4	\N	\N	80	\N	3	\N	3	0	Bad	1
5	\N	\N	90	\N	5	\N	6	0	Bad	1
6	\N	\N	90	\N	0	\N	0	0	Bad	1
7	\N	\N	30	\N	0	\N	0	0	New	1
8	\N	\N	10	\N	0	\N	0	0	New	1
9	\N	\N	20	\N	0	\N	0	0	New	1
\.


---------------------->routerman end!

--
-- Data for Name: save_packet; Type: TABLE DATA; Schema: public; 
--



--
-- PostgreSQL database dump complete
--

