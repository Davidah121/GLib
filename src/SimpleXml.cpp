#include "SimpleXml.h"
#include "StringTools.h"
#include "MathExt.h"

#pragma region XML_ATTRIBUTE

XmlAttribute::XmlAttribute()
{

}

XmlAttribute::~XmlAttribute()
{

}

#pragma endregion


#pragma region XML_NODE

XmlNode::XmlNode()
{
    
}

XmlNode::~XmlNode()
{
    for(XmlNode* child : childNodes)
    {
        delete child;
    }
    childNodes.clear();
    attributes.clear();
}


XmlNode::XmlNode(const XmlNode &other)
{
    this->title = other.title;
    this->value = other.value;
    this->attributes = other.attributes;
    this->isEnd = other.isEnd;
    this->parentNode = other.parentNode;
    
    for(XmlNode* cNode : other.childNodes)
    {
        this->childNodes.push_back( new XmlNode(*cNode) );
    }
}

bool XmlNode::isEndOfSection()
{
    return isEnd;
}

#pragma endregion

#pragma region SIMPLE_XML

SimpleXml::SimpleXml()
{

}

SimpleXml::~SimpleXml()
{
    dispose();
}

void SimpleXml::dispose()
{
    for(XmlNode* n : nodes)
    {
        delete n;
    }
    nodes.clear();
}

void SimpleXml::save(std::string filename)
{
    save(StringTools::toWideString(filename));
}

void SimpleXml::save(std::wstring filename)
{
    SimpleFile f = SimpleFile(filename, SimpleFile::WRITE);

    if(f.isOpen())
    {
        for(XmlNode* v : nodes)
        {
            saveNode(&f, v);
        }

        f.close();
    }
}

void SimpleXml::saveNode(SimpleFile* f, XmlNode* node)
{
    std::wstring line = L"";

    line += L"<";

    //has no child nodes, no attributes, and no value
    if(node->childNodes.size()==0 && node->attributes.size()==0 && node->value==L"")
    {
        line+=L"/";
    }

    line += node->title;

    for(XmlAttribute a : node->attributes)
    {
        line += L" ";
        line += a.name;
        line += L"=\"";
        line += a.value;
        line += L"\"";
    }
    
    //has no child nodes and no value, but has at least one attribute
    if(node->childNodes.size()==0 && node->attributes.size()>0 && node->value==L"")
    {
        line += L"/";
    }
    line += L">";

    f->writeWideString(line);

    if(node->value==L"" && node->childNodes.size()>0)
        f->writeLineBreak();
    else if(node->value!=L"")
        f->writeWideString(node->value);
    else
        f->writeLineBreak();
    
    
    for (XmlNode* v : node->childNodes)
    {
        saveNode(f, v);
    }

    if(node->childNodes.size()>0 || node->value!=L"")
    {
        line = L"</";
        line += node->title;
        line += L">";

        f->writeWideString(line);
        f->writeLineBreak();
    }
}

bool SimpleXml::load(std::string filename)
{
    return load( StringTools::toWideString(filename) );
}

/**
 * No Error Checking currently.
 * Currently Adding error checking and failures
 */
bool SimpleXml::load(std::wstring filename)
{
    SimpleFile f = SimpleFile(filename, SimpleFile::READ);

    bool parsingNode = false;
    std::wstring innerNodeText = L"";

    XmlNode* parentNode = nullptr;
    bool isRecordingText = false;
    bool hitEnd = true;

    if(f.isOpen())
    {
        std::vector<unsigned char> fileBytes = f.readFullFileAsBytes();
        f.close();
        
        fileBytes = removeCommentsAndInvalidChars(fileBytes);

        for(unsigned char byte : fileBytes)
        {
            if(byte=='<' && hitEnd==true)
            {
                //set value of node
                if(parentNode!=nullptr)
                {
                    parentNode->value = innerNodeText;
                }
                innerNodeText = L"";
                isRecordingText = false;
                parsingNode = true;
                hitEnd=false;
            }
            else if(byte=='>' && hitEnd==false)
            {
                parsingNode = false;
                hitEnd=true;

                XmlNode* node = parseXmlLine(innerNodeText);

                if(node==nullptr)
                {
                    dispose();
                    return false;
                }

                innerNodeText = L"";
                isRecordingText = false;
                if(parentNode==nullptr)
                {
                    if(!node->isEndOfSection())
                    {
                        nodes.push_back(node);
                        parentNode = node;
                    }
                    else
                    {
                        if(node->attributes.size()>0)
                        {
                            nodes.push_back(node);
                        }
                        parentNode = node->parentNode;
                    }
                }
                else
                {
                    node->parentNode = parentNode;
                    if(!node->isEndOfSection())
                    {
                        parentNode->childNodes.push_back(node);
                        parentNode = node;
                    }
                    else
                    {
                        if(node->title == parentNode->title && node->isEndOfSection())
                        {
                            parentNode = parentNode->parentNode;
                        }
                        else
                        {
                            parentNode->childNodes.push_back(node);
                            node->parentNode = parentNode;
                        }
                        
                    }
                }
            }
            else
            {
                if(isRecordingText)
                {
                    innerNodeText += byte;
                }
                else
                {
                    if(parsingNode)
                    {
                        if( StringTools::isAlphaNumerial(byte, true, false) || byte == '/' || byte == '?' || byte == '!')
                        {
                            isRecordingText=true;
                            innerNodeText += byte;
                        }
                        else
                        {
                            //error has occurred
                            //Must a number, letter, /, or _ at the start or ? or !
                            dispose();
                            return false;
                        }
                    }
                    else
                    {
                        if(byte > 32)
                        {
                            //valid
                            isRecordingText=true;
                            innerNodeText += byte;
                        }
                    }
                }
                
            }
        }

        for(XmlNode* node : nodes)
        {
            fixParseOnNode(node);
        }

    }
    else
    {
        //StringTools::out << "Could not open file" << StringTools::lineBreak;
        return false;
    }

    return true;
    
}

std::vector<unsigned char> SimpleXml::removeCommentsAndInvalidChars(std::vector<unsigned char> fileBytes)
{
    bool inComment = false;
    bool potentialCommentStuff = false;
    std::vector<unsigned char> newBytes = std::vector<unsigned char>();
    std::string potentialBytes = "";

    for(int i=0; i<fileBytes.size(); i++)
    {
        if(!inComment)
        {
            if(!potentialCommentStuff)
            {
                if(fileBytes[i]=='<')
                {
                    potentialBytes += '<';
                    potentialCommentStuff = true;
                }
                else
                {
                    if(fileBytes[i] >= 32)
                        newBytes.push_back(fileBytes[i]);
                    else if(fileBytes[i]==0x0D || fileBytes[i]==0x0A)
                        newBytes.push_back(' ');
                }
            }
            else
            {
                if(fileBytes[i] >= 32)
                    potentialBytes += fileBytes[i];
                else if(fileBytes[i]==0x0D || fileBytes[i]==0x0A)
                    potentialBytes += ' ';

                if(potentialBytes.size() == 4)
                {
                    if(potentialBytes == "<!--")
                    {
                        inComment = true;
                    }
                    else
                    {
                        newBytes.push_back(potentialBytes[0]);
                        newBytes.push_back(potentialBytes[1]);
                        newBytes.push_back(potentialBytes[2]);
                        newBytes.push_back(potentialBytes[3]);
                    }

                    potentialBytes = "";
                    potentialCommentStuff=false;
                }
            }
        }
        else
        {
            //in comment, check for -->
            if(!potentialCommentStuff)
            {
                if(fileBytes[i] == '-')
                {
                    potentialBytes += '-';
                    potentialCommentStuff = true;
                }
            }
            else
            {
                if(fileBytes[i] >= 32)
                    potentialBytes += fileBytes[i];
                else if(fileBytes[i]==0x0D || fileBytes[i]==0x0A)
                    potentialBytes += ' ';
                
                

                if(potentialBytes.size()==3)
                {
                    if(potentialBytes == "-->")
                    {
                        inComment = false;
                    }
                    
                    potentialBytes = "";
                    potentialCommentStuff = false;
                }
            }
        }
        
    }

    if(!inComment)
    {
        for(int i=0; i<potentialBytes.size(); i++)
        {
            newBytes.push_back( potentialBytes[i] );
        }
    }

    return newBytes;
}

XmlNode* SimpleXml::parseXmlLine(std::wstring line)
{
    //first, get the title. Will always be first and separated by a space from everything else
    XmlNode* node;

    size_t indexOfFirstSpace = line.find_first_of(' ');
    std::wstring title = line.substr(0, indexOfFirstSpace);

    std::wstring attribString = line;

    node = new XmlNode();

    if(line[0] == '/' || line[line.size()-1] == '/')
    {
        node->isEnd = true;

        if(line[0] == '/')
            title = line.substr(1,indexOfFirstSpace);
        
        if(line[line.size()-1]=='/')
        {
            attribString.pop_back();
            title = attribString.substr(0, indexOfFirstSpace);
        }
    }
    else if((line[0] == '?' && line[line.size()-1] == '?') || line[0] == '!')
    {
        //xml declaration
        node->isEnd = true;
        title = line.substr(1, indexOfFirstSpace);
        attribString.pop_back();
    }

    node->title = title;
    
    //split with parameters. No split if surrounded by quotation marks. split by space then by equals
    XmlAttribute attrib = XmlAttribute();
    std::string tempTitle = "";
    std::string tempValue = "";
    bool inQuotes = false;
    bool settingValue = false;

    if(indexOfFirstSpace!=SIZE_MAX)
    {
        for(int i=indexOfFirstSpace+1; i<attribString.size(); i++)
        {
            bool doThing=true;
            if(attribString[i] == '\"')
            {
                inQuotes = !inQuotes;
                doThing = false;
            }

            if(attribString[i] == ' ' && inQuotes == false || i==attribString.size()-1)
            {
                if(i==attribString.size() && doThing)
                {
                    if(!inQuotes)
                    {
                        //invalid
                        delete node;
                        return nullptr;
                    }
                    attrib.value+=attribString[i];
                }

                if(attrib.name != L"")
                    node->attributes.push_back(attrib);
                
                attrib = XmlAttribute();
                settingValue = false;
            }
            else if(doThing)
            {
                if(!inQuotes)
                {
                    if(attribString[i] == '=')
                    {
                        settingValue = true;
                    }
                    else if(settingValue)
                    {
                        if(!inQuotes)
                        {
                            //invalid
                            delete node;
                            return nullptr;
                        }
                        attrib.value += attribString[i];
                    }
                    else
                    {
                        attrib.name += attribString[i];
                    }
                }
                else
                {
                    if(settingValue)
                    {
                        if(!inQuotes)
                        {
                            //invalid
                            delete node;
                            return nullptr;
                        }
                        attrib.value += attribString[i];
                    }
                    else
                    {
                        attrib.name += attribString[i];
                    }
                }
            }
        }
    }
    
    return node;
}

int SimpleXml::parseEscapeString(std::wstring escString)
{
    //Format: &----;
    
    if(escString.front()=='&' && escString.back()==';')
    {
        //valid format
        std::wstring internalString = escString.substr(1, escString.size()-2);
        if(internalString.front() == '#')
        {
            internalString = internalString.substr(1, internalString.size()-1);
            if(internalString.front() == 'x')
            {
                //hex value
                int multiple = 0;
                int charVal = 0;
                for(int i=internalString.size()-1; i>=1; i--, multiple++)
                {
                    charVal += StringTools::base16ToBase10(internalString[i]) * (int)MathExt::pow(16.0, multiple);
                }

                //charVal could be any unicode value. Change later.
                return charVal;
            }
            else
            {
                //decimal number
                return std::stoi( internalString );
            }
        }
        else
        {
            //name
            //only XML predefined names
            if(internalString == L"quot")
            {
                return '"';
            }
            else if(internalString == L"amp")
            {
                return '&';
            }
            else if(internalString == L"apos")
            {
                return '\'';
            }
            else if(internalString == L"lt")
            {
                return '<';
            }
            else if(internalString == L"gt")
            {
                return '>';
            }
        }
        
    }

    return '\0';
}

void SimpleXml::fixParseOnNode(XmlNode* n)
{
    std::wstring actualString = L"";
    std::wstring tempString = L"";
    bool proc = false;
    for(XmlAttribute& k : n->attributes)
    {
        actualString = L"";
        tempString = L"";
        proc = false;
        for(char c : k.value)
        {
            if(!proc)
            {
                if(c != '&')
                {
                    actualString += c;
                }
                else
                {
                    tempString += c;
                    proc=true;
                }
            }
            else
            {
                if(c==';')
                {
                    tempString += ';';
                    int t = parseEscapeString(tempString);
                    actualString += t;
                    proc=false;
                    tempString = L"";
                }
                else
                {
                    tempString += c;
                }
            }
        }
        actualString += tempString;
        k.value = actualString;
    }


    //repeat for the value of the node if it has one
    actualString = L"";
    tempString = L"";
    proc = false;
    for(char c : n->value)
    {
        if(!proc)
        {
            if(c != '&')
            {
                actualString += c;
            }
            else
            {
                tempString += c;
                proc=true;
            }
        }
        else
        {
            if(c==';')
            {
                tempString += ';';
                int t = parseEscapeString(tempString);
                actualString += t;
                proc=false;
                tempString = L"";
            }
            else
            {
                tempString += c;
            }
        }
    }
    actualString += tempString;
    n->value = actualString;
    
    for(XmlNode* q : n->childNodes)
    {
        fixParseOnNode(q);
    }
}

#pragma endregion