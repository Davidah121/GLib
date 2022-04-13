#include "StringTools.h"
#include <string>
#include "Input.h"

#ifdef LINUX
	#include <unistd.h>
	#include <termios.h>
	#include <errno.h>
	
	//Note, works a bit better than the windows equivalent
	int _getwch()
	{
		int buf = 0;
		struct termios old = { 0 };
		fflush(stdout);
		if(tcgetattr(0, &old) < 0)
			perror("tcsetattr()");
		
		old.c_lflag &= ~ICANON; //local modes = Non Canonical Mode
		old.c_lflag &= ~ECHO; //local modes = Disable echo.
		old.c_cc[VMIN] = 1; //control chars (MIN value) = 1
		old.c_cc[VTIME] = 0; //control chars (TIME value) = 0 (No time)
		
		if(tcsetattr(0, TCSANOW, &old) < 0)
			perror("tcsetattr ICANON");
		
		if(read(0, &buf, 4) < 0)
			perror("read()");
		
		old.c_lflag |= ICANON; //local modes = Canonical mode
		old.c_lflag |= ECHO; //local modes = Enable echo
		
		if(tcsetattr(0, TCSADRAIN, &old) < 0)
			perror("tcsetattr ~ICANON");
		
		std::vector<unsigned char> bytes;
		bytes.push_back( buf & 0xFF );

		if(((buf>>8) & 0xFF) > 0)
		{
			bytes.push_back((buf>>8) & 0xFF);
			if(((buf>>16) & 0xFF) > 0)
			{
				bytes.push_back((buf>>16) & 0xFF);
				if(((buf>>24) & 0xFF) > 0)
				{
					bytes.push_back((buf>>24) & 0xFF);
				}
			}
		}
		
		return glib::StringTools::utf8ToChar(bytes);
	}
#else
	#include<io.h>
#endif

#define getch() (_getwch() % 0xFF)
#define getwch() _getwch()

namespace glib
{

	bool StringTools::hasInit = false;
	wchar_t const StringTools::lineBreak = L'\n';

	std::wstreambuf* StringTools::inputBuffer = std::wcin.rdbuf();
	std::wstreambuf* StringTools::outputBuffer = std::wcout.rdbuf();
	std::wstreambuf* StringTools::errorBuffer = std::wcerr.rdbuf();

	void StringTools::init()
	{
		#ifdef LINUX

		#else
			int outRet = _setmode(_fileno(stdout), _O_U16TEXT);
			int inRet = _setmode(_fileno(stdin), _O_U16TEXT);
			int errRet = _setmode(_fileno(stderr), _O_U16TEXT);
		#endif

		std::ios_base::sync_with_stdio(true);

		hasInit = true;
	}

	int StringTools::stringLength(char * text)
	{
		return strlen(text);
	}

	int StringTools::stringLength(const char * text)
	{
		return strlen(text);
	}

	int StringTools::stringLength(wchar_t * text)
	{
		return wcslen(text);
	}

	int StringTools::stringLength(const wchar_t * text)
	{
		return std::wcslen(text);
	}

	bool StringTools::isAlphaNumerial(char v, bool underScoreAllowed, bool dashAllowed)
	{
		if(v >= 48 && v <=57)
		{
			return true;
		}
		if(v >= 65 && v <=90)
		{
			return true;
		}
		if(v >= 97 && v <=122)
		{
			return true;
		}

		if(dashAllowed && v=='-')
		{
			return true;
		}

		if(underScoreAllowed && v=='_')
		{
			return true;
		}

		return false;
	}

	bool StringTools::isValidChar(int v)
	{
		return v>=32 && v<=126;
	}

	int StringTools::convertKeyToAscii(int keyVal, bool shiftHeld)
	{
		if(keyVal>='A' && keyVal<='Z')
		{
			if(shiftHeld)
				return keyVal;
			else
				return keyVal+0x20;
		}
		else if(keyVal>='0' && keyVal<='9')
		{
			if(shiftHeld)
			{
				char conversionTable[10] = {')', '!', '@', '#', '$', '%', '^', '&', '*', '('};
				return conversionTable[ keyVal-'0' ];
			}
			else
			{
				return keyVal;
			}
		}
		else if(keyVal>=Input::KEY_NUMPAD0 && keyVal<=Input::KEY_NUMPAD9)
		{
			return '0'+(keyVal-Input::KEY_NUMPAD0);
		}
		else if(keyVal==Input::KEY_PLUS)
		{
			if(shiftHeld)
				return '+';
			else
				return '=';
		}
		else if(keyVal==Input::KEY_COMMA)
		{
			if(shiftHeld)
				return '<';
			else
				return ',';
		}
		else if(keyVal==Input::KEY_PERIOD)
		{
			if(shiftHeld)
				return '>';
			else
				return '.';
		}
		else if(keyVal==Input::KEY_MINUS)
		{
			if(shiftHeld)
				return '_';
			else
				return '-';
		}
		else if(keyVal==Input::KEY_SPACE)
		{
			return ' ';
		}
		else if(keyVal==Input::KEY_FORWARD_SLASH)
		{
			if(shiftHeld)
				return '?';
			else
				return '/';
		}
		else if(keyVal==Input::KEY_TILDE)
		{
			if(shiftHeld)
				return '~';
			else
				return '`';
		}
		else if(keyVal==Input::KEY_SEMICOLON)
		{
			if(shiftHeld)
				return ':';
			else
				return ';';
		}
		else if(keyVal==Input::KEY_LEFT_SQUARE_BRACKET)
		{
			if(shiftHeld)
				return '{';
			else
				return '[';
		}
		else if(keyVal==Input::KEY_RIGHT_SQUARE_BRACKET)
		{
			if(shiftHeld)
				return '}';
			else
				return ']';
		}
		else if(keyVal==Input::KEY_BACK_SLASH)
		{
			if(shiftHeld)
				return '|';
			else
				return '\\';
		}
		else if(keyVal==Input::KEY_SINGLE_QUOTE)
		{
			if(shiftHeld)
				return '"';
			else
				return '\'';
		}
		else if(keyVal==Input::KEY_NUMPAD_ADD)
		{
			return '+';
		}
		else if(keyVal==Input::KEY_NUMPAD_SUBTRACT)
		{
			return '-';
		}
		else if(keyVal==Input::KEY_NUMPAD_DECIMAL)
		{
			return '.';
		}
		else if(keyVal==Input::KEY_NUMPAD_MULTIPLY)
		{
			return '*';
		}
		else if(keyVal==Input::KEY_NUMPAD_DIVIDE)
		{
			return '/';
		}
		else if(keyVal==Input::KEY_NUMPAD_DECIMAL)
		{
			return '.';
		}

		return -1;
	}
		

	char StringTools::base10ToBase16(char val)
	{
		if(val<10)
			return (char)(48+val);
		else
			return (char)(65+val-10);
	}

	int StringTools::base16ToBase10(char val)
	{
		if(val >= '0' && val <= '9')
		{
			return (int)(val-'0');
		}
		else if(val >= 'A' && val <='F')
		{
			return ((int)(val-'A')) + 10;
		}
		else
		{
			return -1;
		}
	}

	std::vector<unsigned char> StringTools::toUTF8(int c)
	{
		if(c <= 0x7F)
		{
			return {(unsigned char)c};
		}
		else if(c <= 0x7FF)
		{
			unsigned char c1 = 0b11000000;
			unsigned char c2 = 0b10000000;
			c1 += (c >> 6);
			c2 += c & 0b00111111;

			return {c1, c2};
		}
		else if(c <= 0xFFFF)
		{
			unsigned char c1 = 0b11100000;
			unsigned char c2 = 0b10000000;
			unsigned char c3 = 0b10000000;
			c1 += (c >> 12);
			c2 += (c >> 6) & 0b00111111;
			c3 += c & 0b00111111;
			return {c1, c2, c3};
		}
		else
		{
			unsigned char c1 = 0b11110000;
			unsigned char c2 = 0b10000000;
			unsigned char c3 = 0b10000000;
			unsigned char c4 = 0b10000000;

			c1 += (c >> 18);
			c2 += (c >> 12) & 0b00111111;
			c3 += (c >> 6) & 0b00111111;
			c4 += c & 0b00111111;
			return {c1, c2, c3, c4};
		}
	}

	int StringTools::utf8ToChar(std::vector<unsigned char> utf8Char)
	{
		BinarySet b;
		b.setBitOrder(BinarySet::RMSB);
		b.setValues(utf8Char.data(), utf8Char.size());

		BinarySet result;

		int i = 0;

		while(i<b.size())
		{
			if(!b.getBit(i))
			{
				i++;
				int count = 8 - (i % 8);

				for(int k=0; k<count; k++)
				{
					result.add( b.getBit(i) );
					i++;
				}
			}
			else
			{
				i++;
			}
		}

		if(result.size()>0)
			return result.getBits(0, result.size(), true);
		else
			return 0;
	}

	std::vector<std::string> StringTools::splitString(std::string s, const char delim, bool removeEmpty)
	{
		std::vector<std::string> stringArray = std::vector<std::string>();

		std::string temp = "";

		int i = 0;
		while (i < s.size())
		{
			if (s.at(i) != delim)
			{
				temp += s.at(i);
			}
			else
			{
				if(removeEmpty)
				{
					if(temp!="")
						stringArray.push_back(temp);
				}
				else
				{
					stringArray.push_back(temp);
				}
				
				temp = "";
			}
			i++;
		}

		stringArray.push_back(temp);

		return stringArray;
	}

	std::vector<std::string> StringTools::splitStringMultipleDeliminators(std::string s, std::string delim, bool removeEmpty)
	{
		std::vector<std::string> stringArray = std::vector<std::string>();

		std::string temp = "";

		int i = 0;
		int dSize = delim.size();

		while (i < s.size())
		{
			bool valid = true;

			for(int x=0; x<dSize; x++)
			{
				if (s.at(i) == delim[x])
				{
					valid = false;
				}
			}

			if (valid)
			{
				temp += s.at(i);
			}
			else
			{
				if(removeEmpty)
				{
					if(temp!="")
						stringArray.push_back(temp);
				}
				else
				{
					stringArray.push_back(temp);
				}
				
				temp = "";
			}
			
			i++;
		}

		stringArray.push_back(temp);

		return stringArray;
	}

	std::vector<std::string> StringTools::splitString(std::string s, std::string delim, bool removeEmpty)
	{
		std::vector<std::string> stringArray = std::vector<std::string>();

		std::string temp = "";
		std::string otherString = "";

		int dSize = delim.size();

		int i = 0;
		int count = 0;

		while (i < s.size())
		{
			if (s.at(i) != delim[0])
			{
				temp += s.at(i);
				i++;
			}
			else
			{
				if (dSize + i > s.size())
				{
					//can't contain the substring.
					temp+=s.at(i);
					i++;
					continue;
				}

				while (count < dSize)
				{
					if (s.at(i + count) == delim[count])
					{
						otherString += delim[count];
					}
					else
					{
						break;
					}
					count++;
				}

				if (count == dSize)
				{
					//Found subString
					if(removeEmpty)
					{
						if(temp!="")
							stringArray.push_back(temp);
					}
					else
					{
						stringArray.push_back(temp);
					}
					
					temp = "";
					i += count;

					count = 0;
					otherString = "";
				}
				else
				{
					temp += otherString;
					i += count;
					count = 0;
					otherString = "";
				}

			}
		}

		stringArray.push_back(temp);

		return stringArray;
	}


	std::vector<std::wstring> StringTools::splitString(std::wstring s, const wchar_t delim, bool removeEmpty)
	{
		std::vector<std::wstring> stringArray = std::vector<std::wstring>();

		std::wstring temp = L"";

		int i = 0;
		while (i < s.size())
		{
			if (s.at(i) != delim)
			{
				temp += s.at(i);
			}
			else
			{
				if(removeEmpty)
				{
					if(temp!=L"")
						stringArray.push_back(temp);
				}
				else
				{
					stringArray.push_back(temp);
				}
				
				temp = L"";
			}
			i++;
		}

		stringArray.push_back(temp);

		return stringArray;
	}

	std::vector<std::wstring> StringTools::splitStringMultipleDeliminators(std::wstring s, std::wstring delim, bool removeEmpty)
	{
		std::vector<std::wstring> stringArray = std::vector<std::wstring>();

		std::wstring temp = L"";

		int i = 0;
		int dSize = delim.size();

		while (i < s.size())
		{
			bool valid = true;

			for(int x=0; x<dSize; x++)
			{
				if (s.at(i) == delim[x])
				{
					valid = false;
				}
			}

			if (valid)
			{
				temp += s.at(i);
			}
			else
			{
				if(removeEmpty)
				{
					if(temp!=L"")
						stringArray.push_back(temp);
				}
				else
				{
					stringArray.push_back(temp);
				}
				
				temp = L"";
			}
			
			i++;
		}

		stringArray.push_back(temp);

		return stringArray;
	}

	std::vector<std::wstring> StringTools::splitString(std::wstring s, std::wstring delim, bool removeEmpty)
	{
		std::vector<std::wstring> stringArray = std::vector<std::wstring>();

		std::wstring temp = L"";
		std::wstring otherString = L"";

		int dSize = delim.size();

		int i = 0;
		int count = 0;

		while (i < s.size())
		{
			if (s.at(i) != delim[0])
			{
				temp += s.at(i);
				i++;
			}
			else
			{
				if (dSize + i > s.size())
				{
					//can't contain the substring.
					temp+=s.at(i);
					i++;
					continue;
				}

				while (count < dSize)
				{
					if (s.at(i + count) == delim[count])
					{
						otherString += delim[count];
					}
					else
					{
						break;
					}
					count++;
				}

				if (count == dSize)
				{
					//Found subString
					if(removeEmpty)
					{
						if(temp!=L"")
							stringArray.push_back(temp);
					}
					else
					{
						stringArray.push_back(temp);
					}
					
					temp = L"";
					i += count;

					count = 0;
					otherString = L"";
				}
				else
				{
					temp += otherString;
					i += count;
					count = 0;
					otherString = L"";
				}

			}
		}

		stringArray.push_back(temp);

		return stringArray;
	}


	int StringTools::toInt(std::string s)
	{
		return std::stoi(s.c_str());
	}

	long StringTools::toLong(std::string s)
	{
		return std::stol(s.c_str());
	}

	double StringTools::toDouble(std::string s)
	{
		return std::stod(s.c_str());
	}

	float StringTools::toFloat(std::string s)
	{
		return std::stof(s.c_str());
	}

	int StringTools::toInt(std::wstring s)
	{
		return std::stoi(s.c_str());
	}

	long StringTools::toLong(std::wstring s)
	{
		return std::stol(s.c_str());
	}

	double StringTools::toDouble(std::wstring s)
	{
		return std::stod(s.c_str());
	}

	float StringTools::toFloat(std::wstring s)
	{
		return std::stof(s.c_str());
	}

	std::wstring StringTools::getWideString()
	{
		if (!hasInit)
		{
			init();
		}

		std::wstring temp = L"";
		std::getline(std::wcin, temp);
		return temp;
	}

	std::string StringTools::getString()
	{
		std::wstring temp = L"";
		std::getline(std::wcin, temp);
		std::string text = StringTools::toCString(temp);

		return text;
	}

	std::string StringTools::getHiddenString(bool showAsterisk)
	{
		std::string text = "";
		while(true)
		{
			int c = getch();
			if(c == 0x0D || c == 0x0A)
			{
				if(showAsterisk)
					std::wcout << "\n";
				break;
			}
			else if(c == 0x08)
			{
				if(showAsterisk)
				{
					//move cursor back one
					std::wcout << "\x1B[D";
					//clear to end of line
					std::wcout << "\x1B[K";
				}

				if(text.size()>0)
					text.pop_back();
			}
			else if(c >= 32)
			{
				if(showAsterisk)
					std::wcout << "*";
				text += c;
			}
		}

		return text;

	}

	std::wstring StringTools::getHiddenWideString(bool showAsterisk)
	{
		std::wstring text = L"";
		while(true)
		{
			int c = getwch();
			if(c == 0x0D || c == 0x0A)
			{
				if(showAsterisk)
					std::wcout << "\n";
				break;
			}
			else if(c == 0x08)
			{
				if(showAsterisk)
				{
					//move cursor back one
					std::wcout << "\x1B[D";
					//clear to end of line
					std::wcout << "\x1B[K";
				}

				if(text.size()>0)
					text.pop_back();
			}
			else if(c >= 32)
			{
				if(showAsterisk)
					std::wcout << "*";
				text += c;
			}
		}

		return text;
	}

	char StringTools::getChar()
	{
		std::string temp = getString();
		if(temp.empty())
			return 0;
		else
			return temp[0];
	}

	wchar_t StringTools::getWideChar()
	{
		if(!hasInit)
			init();
		
		std::wstring temp = getWideString();
		if(temp.empty())
			return 0;
		else
			return temp[0];
	}

	int StringTools::getInt()
	{
		std::wstring temp;
		std::getline(std::wcin, temp);
		return toInt(temp);
	}

	float StringTools::getFloat()
	{
		std::wstring temp;
		std::getline(std::wcin, temp);
		return toFloat(temp);
	}

	char StringTools::getCharLessLock(bool noFunctionKeys)
	{
		if(noFunctionKeys)
		{
			while(true)
			{
				int c = getch();

				if( c == 0xE0 || c == 0x00 )
				{
					if(c == 0xE0)
					{
						//Could be a valid character.
						//Is a valid character if no character appears after
						//however, there is no way to determine if there is another character
						//without blocking with another getwch call
					}
					getch();
				}
				else if(c >= 0x20)
				{
					return c;
				}
			}
		}
		else
		{
			return getch();
		}
	}

	int StringTools::getWideCharLessLock(bool noFunctionKeys)
	{
		if(noFunctionKeys)
		{
			while(true)
			{
				int c = getwch();
				if( c == 0xE0 || c == 0x00 )
				{
					if(c == 0xE0)
					{
						//Could be a valid character.
						//Is a valid character if no character appears after
						//however, there is no way to determine if there is another character
						//without blocking with another getwch call
					}
					c = getwch();
				}
				else if(c >= 0x20)
				{
					return c;
				}
			}
		}
		else
		{
			return getwch();
		}
	}

	void StringTools::findLongestMatch(std::string base, std::string match, int* index, int* length)
	{
		StringTools::KMP(base.c_str(), base.size(), match.c_str(), match.size(), index, length);
	}

	std::string StringTools::formatStringInternal(std::string text, va_list orgArgs)
	{
		std::string finalText = "";
		std::vector<std::string> splits = splitString(text, "%ls", false);

		va_list args;
		va_copy(args, orgArgs);

		int i=0;

		while(i<splits.size())
		{
			std::string str = splits[i];

			int size = vsnprintf(nullptr, 0, str.c_str(), args);
			size++;

			char* nText = new char[size];
			
			vsnprintf(nText, size, str.c_str(), args);
			finalText += nText;

			delete[] nText;
			
			int count = 0;
			size_t loc = 0;
			
			while(true)
			{
				size_t nLoc = str.find('%', loc);
				if(nLoc != SIZE_MAX)
				{
					loc = nLoc;
					
					while(loc < str.size())
					{
						loc++;
						//read till flag
						if(str[loc] == 'd' || str[loc] == 'i' || str[loc] == 'u' || str[loc] == 'o'
						|| str[loc] == 'x' || str[loc] == 'X' || str[loc] == 'c')
						{
							va_arg(args, size_t);
							count++;
							break;
						}
						else if(str[loc] == 'f' || str[loc] == 'F' || str[loc] == 'e' || str[loc] == 'E'
						|| str[loc] == 'g' || str[loc] == 'G' || str[loc] == 'a' || str[loc] == 'A')
						{
							va_arg(args, long double);
							count++;
							break;
						}
						else if(str[loc] == 's')
						{
							//should always be char*
							va_arg(args, char*);
							count++;
							break;
						}
						else if(str[loc] == 'p')
						{
							va_arg(args, void*);
							count++;
							break;
						}
						else if(str[loc] == 'n')
						{
							va_arg(args, void*);
							count++;
							break;
						}
						else if(str[loc] == L'%')
						{
							break;
						}
						else if(str[loc] == '*')
						{
							va_arg(args, int);
							count++;
						}
					}
				}
				else
				{
					break;
				}
			}

			if(i < splits.size()-1)
			{
				std::wstring delayedStr = va_arg(args, wchar_t*);
				finalText += StringTools::toCString(delayedStr);
				count++;
			}

			i++;
		}

		va_end(args);

		return finalText;
	}

	std::string StringTools::formatString(std::string text, ...)
	{
		std::string finalText = "";

		va_list args;
		va_start(args, text);

		finalText = StringTools::formatStringInternal(text, args);

		va_end(args);

		return finalText;
	}

	std::wstring StringTools::formatWideStringInternal(std::wstring text, va_list orgArgs)
	{
		std::wstring finalText = L"";
		std::vector<std::wstring> splits = splitString(text, L"%s", false);

		va_list args;
		va_copy(args, orgArgs);

		int i = 0;
		while(i<splits.size())
		{
			std::wstring str = splits[i];

			int size = vswprintf(nullptr, 0, str.c_str(), args);
			size++;

			wchar_t* nText = new wchar_t[size];
			
			vswprintf(nText, size, str.c_str(), args);
			finalText += nText;

			delete[] nText;
			
			int count = 0;
			size_t loc = 0;
			
			while(true)
			{
				size_t nLoc = str.find(L'%', loc);
				if(nLoc != SIZE_MAX)
				{
					loc = nLoc;
					
					while(loc < str.size())
					{
						loc++;
						//read till flag
						if(str[loc] == L'd' || str[loc] == L'i' || str[loc] == L'u' || str[loc] == L'o'
						|| str[loc] == L'x' || str[loc] == L'X' || str[loc] == L'c')
						{
							va_arg(args, size_t);
							count++;
							break;
						}
						else if(str[loc] == L'f' || str[loc] == L'F' || str[loc] == L'e' || str[loc] == L'E'
						|| str[loc] == L'g' || str[loc] == L'G' || str[loc] == L'a' || str[loc] == L'A')
						{
							va_arg(args, long double);
							count++;
							break;
						}
						else if(str[loc] == L's')
						{
							//should always be wchar_t*
							va_arg(args, wchar_t*);
							count++;
							break;
						}
						else if(str[loc] == L'p')
						{
							va_arg(args, void*);
							count++;
							break;
						}
						else if(str[loc] == L'n')
						{
							va_arg(args, void*);
							count++;
							break;
						}
						else if(str[loc] == L'%')
						{
							break;
						}
						else if(str[loc] == L'*')
						{
							va_arg(args, int);
							count++;
						}
					}
				}
				else
				{
					break;
				}
			}

			if(i < splits.size()-1)
			{
				std::string delayedStr = va_arg(args, char*);
				finalText += StringTools::toWideString(delayedStr);
				count++;
			}

			i++;
		}

		va_end(args);

		return finalText;
	}

	std::wstring StringTools::formatWideString(std::wstring text, ...)
	{
		std::wstring finalText = L"";

		va_list args;
		va_start(args, text);

		finalText = StringTools::formatWideStringInternal(text, args);

		va_end(args);

		return finalText;
	}

	void StringTools::clearConsole(bool clearScrollBuffer)
	{
		if(clearScrollBuffer)
			StringTools::print("\x1B[2J\x1B[3J\x1B[H");
		else
			StringTools::print("\x1B[2J\x1B[H");
	}

	void StringTools::moveConsoleCursor(int horizontal, int vertical, bool absolute)
	{
		if(absolute)
		{
			int realHVal = (horizontal>=1) ? horizontal : 1;
			int realVVal = (vertical>=1) ? vertical : 1;
			
			StringTools::print("\x1B[%d;%dH", realVVal, realHVal);
		}
		else
		{
			if(horizontal>0)
				StringTools::print("\x1B[%dC", horizontal);
			else if(horizontal<0)
				StringTools::print("\x1B[%dD", std::abs(horizontal));

			if(vertical>0)
				StringTools::print("\x1B[%dB", vertical);
			else if(vertical<0)
				StringTools::print("\x1B[%dA", std::abs(vertical));
		}
	}

	void StringTools::eraseConsoleLine(bool eraseFromCursor)
	{
		if(eraseFromCursor)
			StringTools::print("\x1B[K");
		else
			StringTools::print("\x1B[2K\r");
	}

	void StringTools::reroutOutput(std::wstreambuf* file)
	{
		std::wcout.rdbuf(file);
	}

	void StringTools::reroutInput(std::wstreambuf* file)
	{
		std::wcin.rdbuf(file);
	}

	void StringTools::reroutErrorOutput(std::wstreambuf* file)
	{
		std::wcerr.rdbuf(file);
	}

	void StringTools::resetOutputInputStreams()
	{
		std::wcin.rdbuf(inputBuffer);
		std::wcout.rdbuf(outputBuffer);
		std::wcerr.rdbuf(errorBuffer);
	}

} //NAMESPACE glib END