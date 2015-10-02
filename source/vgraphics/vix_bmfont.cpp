/*
	The MIT License(MIT)

	Copyright(c) 2015 Vixen Team, Matt Guerrette

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files(the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions :
	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#include <vix_bmfont.h>
#include <vix_tinyxml.h>
#include <vix_debugutil.h>
#include <vix_resourcemanager.h>

namespace Vixen {

    BMFont::BMFont()
    {

    }

    BMFont::~BMFont()
    {
        STLVEC_DELETE(m_textures);
    }

    bool BMFont::VInitFromFile(File* file)
    {
        using namespace tinyxml2;

        XMLDOC document;
        XMLError err = document.LoadFile(file->Handle());
        UString errString;
        if (XMLErrCheck(err, errString))
        {
            DebugPrintF(VTEXT("BMFont File failed to load\n"));
            return false;
        }

        BMFont::ReadFontInfo(document, m_fontFile);
        BMFont::ReadFontCommon(document, m_fontFile);
        BMFont::ReadFontPages(document, m_fontFile);
        BMFont::ReadFontChars(document, m_fontFile);

        for (FontChar& c : m_fontFile.chars)
        {
            UChar _c = (UChar)c.id;
            m_charMap[_c] = c;
        }

        for (auto& page : m_fontFile.pages) {
            UString texturePath = page.file;
            ITexture* texture = ResourceManager::OpenTexture(texturePath);
            if (texture)
                m_textures.push_back(texture);
        }

        return true;
    }

    ITexture* BMFont::VPageTexture(size_t index)
    {
        if (index > m_textures.size())
            return NULL;

        return m_textures[index];
    }

    bool BMFont::VFindChar(UChar c, FontChar& fontChar)
    {
        IFont::CharMap::iterator it = m_charMap.find(c);
        if (it != m_charMap.end())
        {
            fontChar = it->second;
            return true;
        }
        else
            return false;
    }

    float BMFont::VLineHeight()
    {
        return m_fontFile.common.lineHeight;
    }


    void BMFont::ReadFontInfo(XMLDOC& doc, BMFontFile& file)
    {
        using namespace tinyxml2;
        XMLElement* fontElement = doc.FirstChildElement("font");
        XMLElement* infoElement = fontElement->FirstChildElement("info");

        /*Populate font info struct of file with parsed info*/
        BMFontInfo& info = file.info;

        const char* _face = infoElement->Attribute("face");
        const char* _charset = infoElement->Attribute("charset");
        const char* _padding = infoElement->Attribute("padding");
        const char* _spacing = infoElement->Attribute("spacing");
        UString spacing;
#ifdef UNICODE
        UConverter cv;
        info.face = cv.from_bytes(_face);
        info.charset = cv.from_bytes(_charset);
        info.padding = cv.from_bytes(_padding);
        spacing = cv.from_bytes(_spacing);
#else
        info.face = _face;
        info.charset = _charset;
        info.padding = _padding;
        spacing = _spacing;
#endif
        info.size = infoElement->IntAttribute("size");
        info.bold = infoElement->IntAttribute("bold");
        info.italic = infoElement->IntAttribute("italic");
        info.unicode = infoElement->IntAttribute("unicode");
        info.stretchH = infoElement->IntAttribute("stretchH");
        info.smooth = infoElement->IntAttribute("smooth");
        info.antiAliasing = infoElement->IntAttribute("aa");
        /*Need to parse the padding values*/
        std::vector<int> paddingValues = parse<int>(info.padding, ',');
        info.padX = paddingValues[0];
        info.padY = paddingValues[1];
        info.padW = paddingValues[2];
        info.padH = paddingValues[3];
        /*Need to parse the spacing values*/
        std::vector<int> spacingValues = parse<int>(spacing, L',');
        info.spacingX = spacingValues[0];
        info.spacingY = spacingValues[1];
        info.outline = infoElement->IntAttribute("outline");
    }

    void BMFont::ReadFontCommon(XMLDOC& doc, BMFontFile& file)
    {
        using namespace tinyxml2;
        XMLElement* fontElement = doc.FirstChildElement("font");
        XMLElement* commonElement = fontElement->FirstChildElement("common");

        /*Populate font common struct of file*/
        BMFontCommon& common = file.common;
        common.lineHeight = commonElement->IntAttribute("lineHeight");
        common.base = commonElement->IntAttribute("base");
        common.scaleW = commonElement->IntAttribute("scaleW");
        common.scaleH = commonElement->IntAttribute("scaleH");
        common.pages = commonElement->IntAttribute("pages");
        common.packed = commonElement->IntAttribute("packed");
        common.alphaChannel = commonElement->IntAttribute("alphaChnl");
        common.redChannel = commonElement->IntAttribute("redChnl");
        common.greenChannel = commonElement->IntAttribute("greenChnl");
        common.blueChannel = commonElement->IntAttribute("blueChnl");
    }

    void BMFont::ReadFontChars(XMLDOC& doc, BMFontFile& file)
    {
        using namespace tinyxml2;
        XMLElement* fontElement = doc.FirstChildElement("font");
        XMLElement* charsElement = fontElement->FirstChildElement("chars");
        int count = charsElement->IntAttribute("count");

        /*Populate font file chars collection*/
        std::vector<FontChar>& chars = file.chars;
        XMLElement* charElement = charsElement->FirstChildElement("char");
        for (int i = 0; i < count; i++)
        {

            FontChar c;
            c.id = charElement->IntAttribute("id");
            c.x = charElement->IntAttribute("x");
            c.y = charElement->IntAttribute("y");
            c.width = charElement->IntAttribute("width");
            c.height = charElement->IntAttribute("height");
            c.xOffset = charElement->IntAttribute("xoffset");
            c.yOffset = charElement->IntAttribute("yoffset");
            c.xAdvance = charElement->IntAttribute("xadvance");
            c.page = charElement->IntAttribute("page");
            c.channel = charElement->IntAttribute("chnl");
            chars.push_back(c);

            /*move to next char element*/
            charElement = charElement->NextSiblingElement("char");
        }
    }

    void BMFont::ReadFontPages(XMLDOC& doc, BMFontFile& file)
    {
        using namespace tinyxml2;
        XMLElement* fontElement = doc.FirstChildElement("font");
        XMLElement* pagesElement = fontElement->FirstChildElement("pages");

        /*Populate font file pages collection*/
        std::vector<BMFontPage>& pages = file.pages;
        XMLElement* pageElement = pagesElement->FirstChildElement("page");
        while (pageElement)
        {
            BMFontPage p;
            p.id = pageElement->IntAttribute("id");
            const char* _file = pageElement->Attribute("file");

#ifdef UNICODE
            UConverter cv;
            p.file = cv.from_bytes(_file);
#else
            p.file = _file;
#endif
            pages.push_back(p);

            /*Try and move to next page element*/
            pageElement = pageElement->NextSiblingElement("page");
        }
    }
}