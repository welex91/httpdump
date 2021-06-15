#include <stdio.h>
#include <string.h>
#include "parser.h"
#include "http_conversation.h"

int
message_cmp(message_t *m1, message_t *m2)
{
    int i, header_cmp = 1;

    if (!m1 && !m2)
        return 1;
    
    if (m1->h_count != m2->h_count)
        return 0;
    
    for (i = 0; i < m1->h_count; i++)
        header_cmp = header_cmp
            && !strcmp(m1->headers[i].name, m2->headers[i].name)
            && !strcmp(m1->headers[i].value, m2->headers[i].value);
    
    return header_cmp
        && m1->method == m2->method
        && m1->version == m2->version
        && !(!m1->url != !m2->url)
        && (m1->url && m2->url ? !strcmp(m1->url, m2->url) : 1)
        && !(!m1->query_string != !m2->query_string)
        && (m1->query_string && m2->query_string
            ? !strcmp (m1->query_string, m2->query_string) : 1)
        && m1->length == m2->length
        && !(!m1->body != !m2->body)
        && (m1->body && m2->body
            ? !memcmp(m1->body , m2->body, m1->length) : 1);
}


int
test1()
{
    char request[] = "GET /coreservices/getAds?cl=3&appId=tmzus_iphone&mt=display&pl=inpage&w=320&h=50&delivery=xml&t=a14ae24ace1b2b7 HTTP/1.1\r\nHost: 10.100.60.14:8080\r\nAccept-Language: en-us\r\nX-rnmd-timezone: America/New_York\r\nAccept-Encoding: gzip\r\nUser-Agent: Mozilla/5.0 (iPhone; CPU iPhone OS 7_0_2 like Mac OS X) AppleWebKit/537.51.1 (KHTML, like Gecko) Mobile/11A501\r\nAccept: */*\r\nX-rnmd-networktype: 3G\r\nuser: SYN_IOS0.0.0.0.lZeflZWflZWflZWflZWflZU_vSnax_tmzus_iphone_70.192.67.128\r\nX-rnmd-don: /cMT7h9Cyhib4wq6HSmQsu9DifN+BZeZxZ5ajcZkQno=\r\nX-rnmd-ua: RhythmSDK-tmzus_iphone-5.3.2.4\r\nCookie: personCookie=0.0.0.0.SYN_IOS0.0.0.0.lZeflZWflZWflZWflZWflZU_vSnax_tmzus_iphone_70.192.67.128;\r\nX-rnmd-mid: xKxvSvZZSQxTe9N4oGzYOA316UND7a4aQZwRtiIg9dM=\r\nX-hardware-platform: iPhone5,2\r\nX-UIDH: NDY0NzM1MTQ3AHXoif1BLna0wSWinR1NlAi848Vz4fR1Kqfy1zDZCO2G\r\nX-Forwarded-For: 70.192.67.128, 10.100.40.253\r\nX-rnmd-trackoptout: no\r\nx-rnmd-pc: 0.0.0.0\r\nX-WEBID: SYN_IOS0.0.0.0.lZeflZWflZWflZWflZWflZU_vSnax_tmzus_iphone_70.192.67.128\r\nX-Forwarded-Host: ads.geo.rnmd.net\r\nX-Forwarded-Server: sccprodads1.scc1.rnmd.net\r\nConnection: close\r\n\r\n";

    header_t h_request[] = {
        {"Host", "10.100.60.14:8080"},
        {"Accept-Language", "en-us"},
        {"X-rnmd-timezone", "America/New_York"},
        {"Accept-Encoding", "gzip"},
        {"User-Agent", "Mozilla/5.0 (iPhone; CPU iPhone OS 7_0_2 like Mac OS X) AppleWebKit/537.51.1 (KHTML, like Gecko) Mobile/11A501"},
        {"Accept", "*/*"},
        {"X-rnmd-networktype", "3G"},
        {"user", "SYN_IOS0.0.0.0.lZeflZWflZWflZWflZWflZU_vSnax_tmzus_iphone_70.192.67.128"},
        {"X-rnmd-don", "/cMT7h9Cyhib4wq6HSmQsu9DifN+BZeZxZ5ajcZkQno="},
        {"X-rnmd-ua", "RhythmSDK-tmzus_iphone-5.3.2.4"},
        {"Cookie", "personCookie=0.0.0.0.SYN_IOS0.0.0.0.lZeflZWflZWflZWflZWflZU_vSnax_tmzus_iphone_70.192.67.128;"},
        {"X-rnmd-mid", "xKxvSvZZSQxTe9N4oGzYOA316UND7a4aQZwRtiIg9dM="},
        {"X-hardware-platform", "iPhone5,2"},
        {"X-UIDH", "NDY0NzM1MTQ3AHXoif1BLna0wSWinR1NlAi848Vz4fR1Kqfy1zDZCO2G"},
        {"X-Forwarded-For", "70.192.67.128, 10.100.40.253"},
        {"X-rnmd-trackoptout", "no"},
        {"x-rnmd-pc", "0.0.0.0"},
        {"X-WEBID", "SYN_IOS0.0.0.0.lZeflZWflZWflZWflZWflZU_vSnax_tmzus_iphone_70.192.67.128"},
        {"X-Forwarded-Host", "ads.geo.rnmd.net"},
        {"X-Forwarded-Server", "sccprodads1.scc1.rnmd.net"},
        {"Connection", "close"}
    };

    
    message_t m_request = {
        GET, ONE_ONE, sizeof(h_request)/sizeof(*h_request), "/coreservices/getAds", "cl=3&appId=tmzus_iphone&mt=display&pl=inpage&w=320&h=50&delivery=xml&t=a14ae24ace1b2b7",
        0,
        NULL,
        h_request
    };
    
    char reply[] = "HTTP/1.1 200 OK\r\nServer: Apache-Coyote/1.1\r\nCache-Control: no-cache\r\nContent-Type: application/xml\r\nContent-Length: 880\r\nDate: Tue, 08 Oct 2013 20:58:09 GMT\r\nConnection: close\r\n\r\n<adresponse requestId=\"10100601081381265890024334\">\n<ad w=\"320\" id=\"2203140\" type=\"html\" h=\"50\" url=\"http://rhythmctv.rnmd.net/iphone/microsites/pubops/rubicon/rubicon-banner-test.html?requestId=10100601081381265890024334&amp;adId=2203140&amp;user=SYN_IOS0.0.0.0.lZeflZWflZWflZWflZWflZU_vSnax_tmzus_iphone_70.192.67.128&amp;appId=tmzus_iphone\" clickThruBackground=\"FFFFFFFF\">\n<impression>\n<url source=\"Rhythm\">\nhttp://ads.rnmd.net/adAck?requestId=10100601081381265890024334&amp;id=2203140&amp;user=SYN_IOS0.0.0.0.lZeflZWflZWflZWflZWflZU_vSnax_tmzus_iphone_70.192.67.128&amp;appId=tmzus_iphone\n</url>\n</impression>\n<clickTracking>\n<url source=\"Rhythm\">\nhttp://ads.rnmd.net/adClick?requestId=10100601081381265890024334&amp;id=2203140&amp;user=SYN_IOS0.0.0.0.lZeflZWflZWflZWflZWflZU_vSnax_tmzus_iphone_70.192.67.128&amp;appId=tmzus_iphone\n</url>\n</clickTracking>\n</ad>\n</adresponse>\n";

    header_t h_reply[] = {
        {"Server", "Apache-Coyote/1.1"},
        {"Cache-Control", "no-cache"},
        {"Content-Type", "application/xml"},
        {"Content-Length", "880"},
        {"Date", "Tue, 08 Oct 2013 20:58:09 GMT"},
        {"Connection", "close"}
    };

    message_t m_reply = {
        OK, ONE_ONE, sizeof(h_reply)/sizeof(*h_reply), NULL, "OK",
        880,
        "<adresponse requestId=\"10100601081381265890024334\">\n<ad w=\"320\" id=\"2203140\" type=\"html\" h=\"50\" url=\"http://rhythmctv.rnmd.net/iphone/microsites/pubops/rubicon/rubicon-banner-test.html?requestId=10100601081381265890024334&amp;adId=2203140&amp;user=SYN_IOS0.0.0.0.lZeflZWflZWflZWflZWflZU_vSnax_tmzus_iphone_70.192.67.128&amp;appId=tmzus_iphone\" clickThruBackground=\"FFFFFFFF\">\n<impression>\n<url source=\"Rhythm\">\nhttp://ads.rnmd.net/adAck?requestId=10100601081381265890024334&amp;id=2203140&amp;user=SYN_IOS0.0.0.0.lZeflZWflZWflZWflZWflZU_vSnax_tmzus_iphone_70.192.67.128&amp;appId=tmzus_iphone\n</url>\n</impression>\n<clickTracking>\n<url source=\"Rhythm\">\nhttp://ads.rnmd.net/adClick?requestId=10100601081381265890024334&amp;id=2203140&amp;user=SYN_IOS0.0.0.0.lZeflZWflZWflZWflZWflZU_vSnax_tmzus_iphone_70.192.67.128&amp;appId=tmzus_iphone\n</url>\n</clickTracking>\n</ad>\n</adresponse>\n",
        h_reply
    };
    
        
    http_conversation_t sample1 = {
        {
            request,
            1106,
            0,
            {0,0}
        },
        {
            reply,
            1059,
            1,
            {0,0}
        }
    };

    int result = parse_conversation(&sample1);
    fprintf(stdout, "Compared sample 1 request: %s\n",
            message_cmp(&sample1.requests[0], &m_request) ? "PASS": "FAIL");
    fprintf(stdout, "Compared sample 1 reply: %s\n",
            message_cmp(&sample1.replies[0], &m_reply) ? "PASS": "FAIL");
    
}


int
test2()
{
    char request[] = "GET /coreservices/getAds?cl=3&appId=mware_solitaire_iphone&mt=all&pl=screenchange&mediaFormat=video/javascript&w=320&h=400&delivery=xml&t=English HTTP/1.1\r\nhost: csads.geo.rnmd.net:8080\r\nAccept: */*\r\nAccept-Encoding: gzip, deflate\r\nAccept-Language: en-us\r\nCookie: personCookie=0.0.0.0.IDFA685F418D-4D91-40C6-B066-CCDC64710AA5;\r\nuser: IDFA685F418D-4D91-40C6-B066-CCDC64710AA5\r\nUser-Agent: Mozilla/5.0 (iPhone; CPU iPhone OS 7_1_2 like Mac OS X) AppleWebKit/537.51.2 (KHTML, like Gecko) Mobile/11D257\r\nX-Forwarded-Host: ads.geo.rnmd.net\r\nX-Forwarded-Server: awsprodads3.west.aws.rnmd.net\r\nX-rnmd-don: OA8QnsAgGJG24SaAxu0dlg==\r\nX-rnmd-idfa: 685F418D-4D91-40C6-B066-CCDC64710AA5\r\nX-rnmd-networktype: wifi\r\nx-rnmd-pc: 0.0.0.0\r\nX-rnmd-trackoptout: no\r\nX-rnmd-ua: RhythmSDK-mware_solitaire_iphone-5.6\r\nX-WEBID: IDFA685F418D-4D91-40C6-B066-CCDC64710AA5\r\nX-Forwarded-For: 65.0.108.63, 10.200.12.12, 10.200.2.83\r\nX-Forwarded-Port: 8080\r\nX-Forwarded-Proto: http\r\nx-haproxy-lb: IDFA685F418D-4D91-40C6-B066-CCDC64710AA5\r\n\r\nGET /coreservices/getAds?cl=3&appId=mware_solitaire_iphone&mt=all&pl=screenchange&mediaFormat=video/javascript&w=320&h=400&delivery=xml&t=English HTTP/1.1\r\nhost: csads.geo.rnmd.net:8080\r\nAccept: */*\r\nAccept-Encoding: gzip, deflate\r\nAccept-Language: en-us\r\nCookie: personCookie=0.0.0.0.IDFA74CF33C3-043F-4C68-8307-FEE12E311590;\r\nuser: IDFA74CF33C3-043F-4C68-8307-FEE12E311590\r\nUser-Agent: Mozilla/5.0 (iPhone; CPU iPhone OS 7_1_2 like Mac OS X) AppleWebKit/537.51.2 (KHTML, like Gecko) Mobile/11D257\r\nX-Forwarded-Host: ads.geo.rnmd.net\r\nX-Forwarded-Server: awsprodads1.west.aws.rnmd.net\r\nX-rnmd-don: O6ueaNpwnGRTzWh4L6Gbqg==\r\nX-rnmd-idfa: 74CF33C3-043F-4C68-8307-FEE12E311590\r\nX-rnmd-networktype: wifi\r\nx-rnmd-pc: 0.0.0.0\r\nX-rnmd-trackoptout: no\r\nX-rnmd-ua: RhythmSDK-mware_solitaire_iphone-5.6\r\nX-WEBID: IDFA74CF33C3-043F-4C68-8307-FEE12E311590\r\nX-Forwarded-For: 65.130.229.182, 10.200.12.12, 10.200.2.81\r\nX-Forwarded-Port: 8080\r\nX-Forwarded-Proto: http\r\nx-haproxy-lb: IDFA74CF33C3-043F-4C68-8307-FEE12E311590\r\n\r\n";

    header_t h_request_1[] = {
        {"host", "csads.geo.rnmd.net:8080"},
        {"Accept", "*/*"},
        {"Accept-Encoding", "gzip, deflate"},
        {"Accept-Language", "en-us"},
        {"Cookie", "personCookie=0.0.0.0.IDFA685F418D-4D91-40C6-B066-CCDC64710AA5;"},
        {"user", "IDFA685F418D-4D91-40C6-B066-CCDC64710AA5"},
        {"User-Agent", "Mozilla/5.0 (iPhone; CPU iPhone OS 7_1_2 like Mac OS X) AppleWebKit/537.51.2 (KHTML, like Gecko) Mobile/11D257"},
        {"X-Forwarded-Host", "ads.geo.rnmd.net"},
        {"X-Forwarded-Server", "awsprodads3.west.aws.rnmd.net"},
        {"X-rnmd-don", "OA8QnsAgGJG24SaAxu0dlg=="},
        {"X-rnmd-idfa", "685F418D-4D91-40C6-B066-CCDC64710AA5"},
        {"X-rnmd-networktype", "wifi"},
        {"x-rnmd-pc", "0.0.0.0"},
        {"X-rnmd-trackoptout", "no"},
        {"X-rnmd-ua", "RhythmSDK-mware_solitaire_iphone-5.6"},
        {"X-WEBID", "IDFA685F418D-4D91-40C6-B066-CCDC64710AA5"},
        {"X-Forwarded-For", "65.0.108.63, 10.200.12.12, 10.200.2.83"},
        {"X-Forwarded-Port", "8080"},
        {"X-Forwarded-Proto", "http"},
        {"x-haproxy-lb", "IDFA685F418D-4D91-40C6-B066-CCDC64710AA5"},
    };

    message_t m_request_1 = {
        GET, ONE_ONE, sizeof(h_request_1)/sizeof(*h_request_1),
        "/coreservices/getAds",
        "cl=3&appId=mware_solitaire_iphone&mt=all&pl=screenchange&mediaFormat=video/javascript&w=320&h=400&delivery=xml&t=English",
        0,
        NULL,
        h_request_1
    };
    
    header_t h_request_2[] = {
        {"host", "csads.geo.rnmd.net:8080"},
        {"Accept", "*/*"},
        {"Accept-Encoding", "gzip, deflate"},
        {"Accept-Language", "en-us"},
        {"Cookie", "personCookie=0.0.0.0.IDFA74CF33C3-043F-4C68-8307-FEE12E311590;"},
        {"user", "IDFA74CF33C3-043F-4C68-8307-FEE12E311590"},
        {"User-Agent", "Mozilla/5.0 (iPhone; CPU iPhone OS 7_1_2 like Mac OS X) AppleWebKit/537.51.2 (KHTML, like Gecko) Mobile/11D257"},
        {"X-Forwarded-Host", "ads.geo.rnmd.net"},
        {"X-Forwarded-Server", "awsprodads1.west.aws.rnmd.net"},
        {"X-rnmd-don", "O6ueaNpwnGRTzWh4L6Gbqg=="},
        {"X-rnmd-idfa", "74CF33C3-043F-4C68-8307-FEE12E311590"},
        {"X-rnmd-networktype", "wifi"},
        {"x-rnmd-pc", "0.0.0.0"},
        {"X-rnmd-trackoptout", "no"},
        {"X-rnmd-ua", "RhythmSDK-mware_solitaire_iphone-5.6"},
        {"X-WEBID", "IDFA74CF33C3-043F-4C68-8307-FEE12E311590"},
        {"X-Forwarded-For", "65.130.229.182, 10.200.12.12, 10.200.2.81"},
        {"X-Forwarded-Port", "8080"},
        {"X-Forwarded-Proto", "http"},
        {"x-haproxy-lb", "IDFA74CF33C3-043F-4C68-8307-FEE12E311590"},
    };
    
    message_t m_request_2 = {
        GET, ONE_ONE,
        sizeof(h_request_2)/sizeof(*h_request_2),
        "/coreservices/getAds",
        "cl=3&appId=mware_solitaire_iphone&mt=all&pl=screenchange&mediaFormat=video/javascript&w=320&h=400&delivery=xml&t=English",
        0,
        NULL,
        h_request_2
    };
    
    char reply[] = "HTTP/1.1 200 OK\r\nServer: Apache-Coyote/1.1\r\nCache-Control: no-cache\r\nX-Rhythm-Version: 1.0\r\nContent-Type: text/xml\r\nContent-Length: 66\r\nDate: Mon, 27 Oct 2014 20:53:12 GMT\r\n\r\n<adresponse requestId=\"10200131061414443193329412\">\n</adresponse>\nHTTP/1.1 200 OK\r\nServer: Apache-Coyote/1.1\r\nCache-Control: no-cache\r\nX-Rhythm-Version: 1.0\r\nContent-Type: text/xml\r\nContent-Length: 66\r\nDate: Mon, 27 Oct 2014 20:55:45 GMT\r\n\r\n<adresponse requestId=\"10200131061414443346093575\">\n</adresponse>\r\n";

    header_t h_reply_1[] = {
        {"Server", "Apache-Coyote/1.1"},
        {"Cache-Control", "no-cache"},
        {"X-Rhythm-Version", "1.0"},
        {"Content-Type", "text/xml"},
        {"Content-Length", "66"},
        {"Date", "Mon, 27 Oct 2014 20:53:12 GMT"}
    };
    message_t m_reply_1 = {
        OK, ONE_ONE, sizeof(h_reply_1)/sizeof(*h_reply_1),
        NULL,
        "OK",
        66,
        "<adresponse requestId=\"10200131061414443193329412\">\n</adresponse>\n",
        h_reply_1
    };
    header_t h_reply_2[] = {
        {"Server", "Apache-Coyote/1.1"},
        {"Cache-Control", "no-cache"},
        {"X-Rhythm-Version", "1.0"},
        {"Content-Type", "text/xml"},
        {"Content-Length", "66"},
        {"Date", "Mon, 27 Oct 2014 20:55:45 GMT"},
    };
    message_t m_reply_2 = {
        OK, ONE_ONE, sizeof(h_reply_2)/sizeof(*h_reply_2),
        NULL,
        "OK",
        66,
        "<adresponse requestId=\"10200131061414443346093575\">\n</adresponse>\r\n",
        h_reply_2
    };
    
    http_conversation_t sample2 = {
        {
            request,
            2023,
            0,
            {0,0}
        },
        {
            reply,
            482,
            1,
            {0,0}
        }
    };

    parse_conversation(&sample2);
    fprintf(stdout, "Compared sample 2 request 1: %s\n",
            message_cmp(&sample2.requests[0], &m_request_1) ? "PASS": "FAIL");
    fprintf(stdout, "Compared sample 2 reply 1: %s\n",
            message_cmp(&sample2.replies[0], &m_reply_1) ? "PASS": "FAIL");
    fprintf(stdout, "Compared sample 2 request 2: %s\n",
            message_cmp(&sample2.requests[1], &m_request_2) ? "PASS": "FAIL");
    fprintf(stdout, "Compared sample 2 reply 2: %s\n",
            message_cmp(&sample2.replies[1], &m_reply_2) ? "PASS": "FAIL");
}


int
test3()
{
    char request[] = "POST /coreservices/adDuration?requestId=10100601131381265888918010&id=2049816&d=314 HTTP/1.1\r\nHost: 10.100.60.14:8080\r\nUser-Agent: Mozilla/5.0 (iPhone; CPU iPhone OS 7_0 like Mac OS X) AppleWebKit/537.51.1 (KHTML, like Gecko) Mobile/11A465\r\nX-rnmd-timezone: America/Los_Angeles\r\nAccept-Language: en-us\r\nAccept-Encoding: gzip, deflate\r\nAccept: */*\r\nX-rnmd-idfa: C97CEFE0-7B70-4012-8C0C-BB4D10DB3731\r\nX-rnmd-networktype: wifi\r\nuser: 0.0.0.0.lZeflZWflZWflZWflZWflZU_\r\nX-rnmd-don: eA2qPaJz5iELe6iCYBRc0XPxn0xk/9xnmjSI067Yz9Y=\r\nX-rnmd-ua: RhythmSDK-cnbc_iphone-5.4\r\nX-rnmd-trackoptout: yes\r\nX-rnmd-mid: xKxvSvZZSQxTe9N4oGzYOA316UND7a4aQZwRtiIg9dM=\r\nCookie: personCookie=0.0.0.0.lZeflZWflZWflZWflZWflZU_\r\nX-hardware-platform: iPhone4,1\r\nVia: HTTP/1.1 cncmspsrvz4ts214.wnsnet.attws.com\r\nX-Forwarded-For: 166.137.209.156, 10.100.40.253\r\nx-rnmd-pc: 0.0.0.0\r\nX-WEBID: 0.0.0.0.lZeflZWflZWflZWflZWflZU_\r\nX-Forwarded-Host: ads.geo.rnmd.net\r\nX-Forwarded-Server: sccprodads1.scc1.rnmd.net\r\nConnection: close\r\nContent-Length: 0\r\n\r\n";

    header_t h_request[] = {
        {"Host", "10.100.60.14:8080"},
        {"User-Agent", "Mozilla/5.0 (iPhone; CPU iPhone OS 7_0 like Mac OS X) AppleWebKit/537.51.1 (KHTML, like Gecko) Mobile/11A465"},
        {"X-rnmd-timezone", "America/Los_Angeles"},
        {"Accept-Language", "en-us"},
        {"Accept-Encoding", "gzip, deflate"},
        {"Accept", "*/*"},
        {"X-rnmd-idfa", "C97CEFE0-7B70-4012-8C0C-BB4D10DB3731"},
        {"X-rnmd-networktype", "wifi"},
        {"user", "0.0.0.0.lZeflZWflZWflZWflZWflZU_"},
        {"X-rnmd-don", "eA2qPaJz5iELe6iCYBRc0XPxn0xk/9xnmjSI067Yz9Y="},
        {"X-rnmd-ua", "RhythmSDK-cnbc_iphone-5.4"},
        {"X-rnmd-trackoptout", "yes"},
        {"X-rnmd-mid", "xKxvSvZZSQxTe9N4oGzYOA316UND7a4aQZwRtiIg9dM="},
        {"Cookie", "personCookie=0.0.0.0.lZeflZWflZWflZWflZWflZU_"},
        {"X-hardware-platform", "iPhone4,1"},
        {"Via", "HTTP/1.1 cncmspsrvz4ts214.wnsnet.attws.com"},
        {"X-Forwarded-For", "166.137.209.156, 10.100.40.253"},
        {"x-rnmd-pc", "0.0.0.0"},
        {"X-WEBID", "0.0.0.0.lZeflZWflZWflZWflZWflZU_"},
        {"X-Forwarded-Host", "ads.geo.rnmd.net"},
        {"X-Forwarded-Server", "sccprodads1.scc1.rnmd.net"},
        {"Connection", "close"},
        {"Content-Length", "0"}        
    };
    message_t m_request[] = {
        POST, ONE_ONE, sizeof(h_request)/sizeof(*h_request),
        "/coreservices/adDuration",
        "requestId=10100601131381265888918010&id=2049816&d=314",
        0,
        NULL,
        h_request
    };
    
    char reply[] = "HTTP/1.1 200 OK\r\nServer: Apache-Coyote/1.1\r\nCache-Control: no-cache\r\nContent-Length: 0\r\nDate: Tue, 08 Oct 2013 20:58:09 GMT\r\nConnection: close\r\n\r\n";
    
    header_t h_reply[] = {
        {"Server", "Apache-Coyote/1.1"},
        {"Cache-Control", "no-cache"},
        {"Content-Length", "0"},
        {"Date", "Tue, 08 Oct 2013 20:58:09 GMT"},
        {"Connection", "close"}
    };
    message_t m_reply = {
        OK, ONE_ONE, sizeof(h_reply)/sizeof(*h_reply),
        NULL,
        "OK",
        0,
        NULL,
        h_reply
    };
    http_conversation_t sample3 = {
        {
            request,
            1015,
            0,
            {0,0}
        },
        {
            reply,
            146,
            1,
            {0,0}
        }
    };
    
    parse_conversation(&sample3);

    fprintf(stdout, "Compared sample 3 request: %s\n",
            message_cmp(&sample3.requests[0], &m_request) ? "PASS": "FAIL");
    fprintf(stdout, "Compared sample 3 reply: %s\n",
            message_cmp(&sample3.replies[0], &m_reply) ? "PASS": "FAIL");
}

int
test4()
{
    char request[] = "POST /coreservices/adServed HTTP/1.1\r\nHost: 10.100.60.14:8080\r\nAccept: */*\r\nuserId: 1374515160276\r\nX-WEBID: 1374515160276\r\nx-rnmd-pc: 33\r\nContent-Type: application/x-www-form-urlencoded\r\nX-Forwarded-For: 208.91.191.36, 10.100.40.253\r\nX-Forwarded-Host: ads.rnmd.net\r\nX-Forwarded-Server: sccprodads1.scc1.rnmd.net\r\nConnection: close\r\nContent-Length: 56\r\n\r\nrtspSID=10100601131381265889197033&downloadCompleted=yes";
    header_t h_request[] = {
        {"Host", "10.100.60.14:8080"},
        {"Accept", "*/*"},
        {"userId", "1374515160276"},
        {"X-WEBID", "1374515160276"},
        {"x-rnmd-pc", "33"},
        {"Content-Type", "application/x-www-form-urlencoded"},
        {"X-Forwarded-For", "208.91.191.36, 10.100.40.253"},
        {"X-Forwarded-Host", "ads.rnmd.net"},
        {"X-Forwarded-Server", "sccprodads1.scc1.rnmd.net"},
        {"Connection", "close"},
        {"Content-Length", "56"}
    };
    message_t m_request[] = {
        POST, ONE_ONE, sizeof(h_request)/sizeof(*h_request),
        "/coreservices/adServed",
        NULL,
        56,
        "rtspSID=10100601131381265889197033&downloadCompleted=yes",
        h_request
    };

    char reply[] = "HTTP/1.1 200 OK\r\nServer: Apache-Coyote/1.1\r\nCache-Control: no-cache\r\nContent-Length: 0\r\nDate: Tue, 08 Oct 2013 20:58:09 GMT\r\nConnection: close\r\n\r\n";
    
    header_t h_reply[] = {
        {"Server", "Apache-Coyote/1.1"},
        {"Cache-Control", "no-cache"},
        {"Content-Length", "0"},
        {"Date", "Tue, 08 Oct 2013 20:58:09 GMT"},
        {"Connection", "close"}
    };
    message_t m_reply = {
        OK, ONE_ONE, sizeof(h_reply)/sizeof(*h_reply),
        NULL,
        "OK",
        0,
        NULL,
        h_reply
    };

    http_conversation_t sample4 = {
        {
            request,
            410,
            0,
            {0,0}
        },
        {
            reply,
            146,
            1,
            {0,0}
        }
    };

    parse_conversation(&sample4);
    fprintf(stdout, "Compared sample 4 request: %s\n",
            message_cmp(&sample4.requests[0], &m_request) ? "PASS": "FAIL");
    fprintf(stdout, "Compared sample 4 reply: %s\n",
            message_cmp(&sample4.replies[0], &m_reply) ? "PASS": "FAIL");
}

int
test5()
{
    char request[] = "GET /vsnaxepg/test/monitor.jspx HTTP/1.0\n\n";
    char reply5[] = "HTTP/1.1 200 OK\r\nDate: Tue, 08 Oct 2013 20:58:11 GMT\r\nServer: Apache-Coyote/1.1\r\nContent-Type: text/plain;charset=UTF-8\r\nContent-Length: 77\r\nVary: Accept-Encoding,User-Agent\r\nConnection: close\r\n\r\nOK\n\nEPG Modified at: Tue Oct 08 19:23:46 UTC 2013\nlocalhost.localdomain:8080\n";
    
    http_conversation_t sample5 = {
        {
            request,
            42,
            0,
            {0,0}
        },
        {
            reply5,
            273,
            1,
            {0,0}
        }
    };
    parse_conversation(&sample5);
    fprintf(stdout, "Compared sample 5 request: %s\n",
            message_cmp(&sample5.requests[0], NULL) ? "PASS": "FAIL");
    fprintf(stdout, "Compared sample 5 reply: %s\n",
            message_cmp(&sample5.replies[0], NULL) ? "PASS": "FAIL");

}

int
test6()
{
    char request[] = "POST /adServed HTTP/1.1\r\nHost: ads.rnmd.net\r\nAccept: */*\r\nuserId: 1374515160276\r\nX-WEBID: 1374515160276\r\nx-rnmd-pc: 33\r\nContent-Length: 56\r\nContent-Type: application/x-www-form-urlencoded\r\nX-Forwarded-For: 208.91.191.36\r\n\r\nrtspSID=10100601131381265889197033&downloadCompleted=yes\r\n\r\n";

    header_t h_request[] = {
            {"Host", "ads.rnmd.net"},
            {"Accept", "*/*"},
            {"userId", "1374515160276"},
            {"X-WEBID", "1374515160276"},
            {"x-rnmd-pc", "33"},
            {"Content-Length", "56"},
            {"Content-Type", "application/x-www-form-urlencoded"},
            {"X-Forwarded-For", "208.91.191.36"}
    };
    message_t m_request = {
        POST, ONE_ONE, sizeof(h_request)/sizeof(*h_request),
        "/adServed", NULL,
        56,
        "rtspSID=10100601131381265889197033&downloadCompleted=yes",
        h_request
    };
    
    char reply[] = "HTTP/1.1 200 OK\r\nDate: Tue, 08 Oct 2013 20:58:09 GMT\r\nServer: Apache-Coyote/1.1\r\nCache-Control: no-cache\r\nContent-Length: 0\r\nConnection: close\r\nContent-Type: text/plain; charset=UTF-8\r\n\r\n\r\n";

    header_t h_reply[] = {
        {"Date", "Tue, 08 Oct 2013 20:58:09 GMT"},
        {"Server", "Apache-Coyote/1.1"},
        {"Cache-Control", "no-cache"},
        {"Content-Length", "0"},
        {"Connection", "close"},
        {"Content-Type", "text/plain; charset=UTF-8"}
    };
    message_t m_reply = {
        OK, ONE_ONE, sizeof(h_reply)/sizeof(*h_reply),
        NULL,
        "OK",
        0,
        NULL,
        h_reply
    };
    
    http_conversation_t sample = {
        {
            request,
            187,
            1,
            {0,0}
        },
        {
            reply,
            187,
            1,
            {0,0}
        }
    };

    
    parse_conversation(&sample);
    fprintf(stdout, "Compared sample 6 request: %s\n", message_cmp(&sample.requests[0], &m_request) ? "PASS": "FAIL");
    fprintf(stdout, "Compared sample 6 reply: %s\n", message_cmp(&sample.replies[0], &m_reply) ? "PASS": "FAIL");
}

int
test7() 
{
    char request[] = "GET /coreservices/adAck?requestId=10106321379107254062308&id=2209012&user=IDFA06DAD01F-D42C-4865-A157-4488D060DB7C&appId=tmzus_iphone HTTP/1.1\r\nHost: 10.10.6.222:8080\r\nAccept: */*\r\nuser: IDFA06DAD01F-D42C-4865-A157-4488D060DB7C\r\nAccept-Language: en-us\r\nAccept-Encoding: gzip, deflate\r\nCookie: personCookie=0.0.0.0.neafkJ2fkpKfkpOf5pWfkZw_\r\nX-rnmd-ua: RhythmSDK-tmzus_iphone-5.4\r\nX-hardware-platform: iPhone3,3\r\nUser-Agent: Mozilla/5.0 (iPhone; CPU iPhone OS 6_1_3 like Mac OS X) AppleWebKit/536.26 (KHTML, like Gecko) Mobile/10B329\r\nX-Forwarded-For: 208.91.190.83, 10.10.20.251\r\nx-rnmd-pc: 0.0.0.0\r\nX-WEBID: IDFA06DAD01F-D42C-4865-A157-4488D060DB7C\r\nX-Forwarded-Host: stageepg.mtv.rnmd.net\r\nX-Forwarded-Server: stageepg1.mtv.rnmd.net\r\nConnection: close\r\n\r\n\r\n";

    header_t h_request[] = {
        {"Host", "10.10.6.222:8080"},
        {"Accept", "*/*"},
        {"user", "IDFA06DAD01F-D42C-4865-A157-4488D060DB7C"},
        {"Accept-Language", "en-us"},
        {"Accept-Encoding", "gzip, deflate"},
        {"Cookie", "personCookie=0.0.0.0.neafkJ2fkpKfkpOf5pWfkZw_"},
        {"X-rnmd-ua", "RhythmSDK-tmzus_iphone-5.4"},
        {"X-hardware-platform", "iPhone3,3"},
        {"User-Agent", "Mozilla/5.0 (iPhone; CPU iPhone OS 6_1_3 like Mac OS X) AppleWebKit/536.26 (KHTML, like Gecko) Mobile/10B329"},
        {"X-Forwarded-For", "208.91.190.83, 10.10.20.251"},
        {"x-rnmd-pc", "0.0.0.0"},
        {"X-WEBID", "IDFA06DAD01F-D42C-4865-A157-4488D060DB7C"},
        {"X-Forwarded-Host", "stageepg.mtv.rnmd.net"},
        {"X-Forwarded-Server", "stageepg1.mtv.rnmd.net"},
        {"Connection", "close"}
    };
    message_t m_request = {
        GET, ONE_ONE, sizeof(h_request)/sizeof(*h_request),
        "/coreservices/adAck", "requestId=10106321379107254062308&id=2209012&user=IDFA06DAD01F-D42C-4865-A157-4488D060DB7C&appId=tmzus_iphone",
        0,
        NULL,
        h_request
    };
    
    char reply[] = "HTTP/1.1 200 OK\r\nServer: Apache-Coyote/1.1\r\nCache-Control: no-cache\r\nContent-Type: image/gif\r\nContent-Length: 43\r\nDate: Fri, 13 Sep 2013 21:20:54 GMT\r\nConnection: close\r\n\r\nGIF89a.............!.......,...........D..;\r\n";

    header_t h_reply[] = {
        {"Server", "Apache-Coyote/1.1"},
        {"Cache-Control", "no-cache"},
        {"Content-Type", "image/gif"},
        {"Content-Length", "43"},
        {"Date", "Fri, 13 Sep 2013 21:20:54 GMT"},
        {"Connection", "close"}
    };

    message_t m_reply = {
        OK, ONE_ONE, sizeof(h_reply)/sizeof(*h_reply),
        NULL,
        "OK",
        43,
        "GIF89a.............!.......,...........D..;",
        h_reply
    };
    
    http_conversation_t sample = {
        {
            request,
            756,
            1,
            {0,0}
        },
        {
            reply,
            215,
            1,
            {0,0}
        }
    };

    
    parse_conversation(&sample);
    fprintf(stdout, "Compared sample 7 request: %s\n", message_cmp(&sample.requests[0], &m_request) ? "PASS": "FAIL");
    fprintf(stdout, "Compared sample 7 reply: %s\n", message_cmp(&sample.replies[0], &m_reply) ? "PASS": "FAIL");
    }

int
main (int argc, char *argv[])
{
    test1();
    test2();
    test3();
    test4();
    test5();
    test6();
    test7();
    
}

/* Local Variables: */
/* mode:c */
/* compile-command: "gcc -O0 -g3 -I.. -I../include -o test_parser test_parser.c parser.c"  */
/* End: */
