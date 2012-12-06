#pragma once

#include <string>
class KeyConverter
{
public:
	KeyConverter();
	virtual ~KeyConverter();

	std::string ConvertToString(const char key) const;
	char ConvertToChar(const std::string& key) const;
	bool CheckNumPad(std::string& ret, const char key) const;
	bool CheckFKeys(std::string& ret,const char key) const;
	bool CheckCommonKeys(std::string& ret,const char key) const;
	bool CheckUncommonKeys(std::string& ret,const char key) const;
private:

};