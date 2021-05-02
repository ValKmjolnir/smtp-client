#ifndef __BASE64_H__
#define __BASE64_H__

char table[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
std::string encode(std::string& s)
{
	std::string str="";
	std::string res="";
	for(int i=0;i<s.length();++i)
		for(int j=0;j<8;++j)
			str.push_back((s[i]&(1<<(7-j)))?'1':'0');
	while(str.length()%6)
		str.push_back('0');
	for(int i=0;i<str.length();i+=6)
	{
		char tmp=0;
		for(int j=0;j<6;++j)
		{
			tmp<<=1;
			tmp+=str[i+j]-'0';
		}
		res.push_back(table[tmp]);
	}
	while(res.length()%4)
		res.push_back('=');
	return res;
}
std::string decode(std::string& s)
{
    std::string str="";
    std::string res="";
	for(int i=0;i<s.length();i+=4)
	{
		std::string t=s.substr(i,4);
		for(int j=0;j<4;++j)
		{
			if(t[j]=='=')
				break;
            char c;
			for(int k=0;k<64;++k)
                if(t[j]==table[k])
                {
                    c=k;
                    break;
                }
			for(int k=0;k<6;++k)
				str+=(c&(1<<(5-k)))?'1':'0';
		}
	}
	for(int i=0;i<str.length();i+=8)
	{
		res.push_back('\0');
		for(int j=0;j<8;++j)
		{
			res[i/8]<<=1;
			res[i/8]+=(str[i+j]-'0');
		}
	}
	return res;
}

#endif