
#
//
//	To avoid including the ad9361 library (just because in
//	Ubuntu 16.04, used for generating an appImage, only
//	an old version is available, the parameters of a filter
//	
//	Fpass     = 1536000 / 2;
//	Fstop     = Fpass * 1.2;
//	wnomTX    = 1.6 * Fstop;  // dummy here
//	wnomRX    = 1536000; // RF bandwidth of analog filter
//
//	were extracted and the filter configuration is loaded

const char * dabFilter =
"RX 3 GAIN -12 DEC 4\n"
"TX 3 GAIN 0 INT 4\n"
"924,226\n"
"-576,-562\n"
"1276,-78\n"
"-710,-1344\n"
"274,-1448\n"
"-1630,-2816\n"
"-1662,-3388\n"
"-3472,-4572\n"
"-3878,-4970\n"
"-5114,-5406\n"
"-5014,-4970\n"
"-5092,-4288\n"
"-3978,-2894\n"
"-2918,-1422\n"
"-1158,254\n"
"188,1530\n"
"1542,2442\n"
"2056,2602\n"
"2176,2216\n"
"1474,1238\n"
"576,90\n"
"-602,-1064\n"
"-1406,-1800\n"
"-1894,-2046\n"
"-1678,-1654\n"
"-1062,-838\n"
"-32,234\n"
"904,1174\n"
"1626,1768\n"
"1752,1782\n"
"1378,1266\n"
"486,332\n"
"-522,-690\n"
"-1432,-1520\n"
"-1840,-1848\n"
"-1688,-1598\n"
"-916,-798\n"
"166,280\n"
"1282,1318\n"
"1988,1950\n"
"2084,1966\n"
"1436,1306\n"
"268,168\n"
"-1112,-1118\n"
"-2202,-2108\n"
"-2642,-2462\n"
"-2176,-1982\n"
"-908,-770\n"
"852,856\n"
"2512,2368\n"
"3518,3244\n"
"3388,3076\n"
"2020,1776\n"
"-354,-408\n"
"-3066,-2874\n"
"-5262,-4828\n"
"-6028,-5460\n"
"-4708,-4184\n"
"-1070,-808\n"
"4560,4360\n"
"11344,10552\n"
"18086,16686\n"
"23502,21604\n"
"26516,24338\n"
"26516,24338\n"
"23502,21604\n"
"18086,16686\n"
"11344,10552\n"
"4560,4360\n"
"-1070,-808\n"
"-4708,-4184\n"
"-6028,-5460\n"
"-5262,-4828\n"
"-3066,-2874\n"
"-354,-408\n"
"2020,1776\n"
"3388,3076\n"
"3518,3244\n"
"2512,2368\n"
"852,856\n"
"-908,-770\n"
"-2176,-1982\n"
"-2642,-2462\n"
"-2202,-2108\n"
"-1112,-1118\n"
"268,168\n"
"1436,1306\n"
"2084,1966\n"
"1988,1950\n"
"1282,1318\n"
"166,280\n"
"-916,-798\n"
"-1688,-1598\n"
"-1840,-1848\n"
"-1432,-1520\n"
"-522,-690\n"
"486,332\n"
"1378,1266\n"
"1752,1782\n"
"1626,1768\n"
"904,1174\n"
"-32,234\n"
"-1062,-838\n"
"-1678,-1654\n"
"-1894,-2046\n"
"-1406,-1800\n"
"-602,-1064\n"
"576,90\n"
"1474,1238\n"
"2176,2216\n"
"2056,2602\n"
"1542,2442\n"
"188,1530\n"
"-1158,254\n"
"-2918,-1422\n"
"-3978,-2894\n"
"-5092,-4288\n"
"-5014,-4970\n"
"-5114,-5406\n"
"-3878,-4970\n"
"-3472,-4572\n"
"-1662,-3388\n"
"-1630,-2816\n"
"274,-1448\n"
"-710,-1344\n"
"1276,-78\n"
"-576,-562\n"
"924,226\n";
