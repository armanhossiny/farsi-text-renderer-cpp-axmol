#include <stdio.h>
#include <string>
#include <map>
#include <vector>
#include <wchar.h>
#include <stdio.h>

/*
*
* Unicode Spaces and character information
* Sources: https://www.compart.com/en/unicode/
*
*/

#define RENDER_SYMBOLS L"<>(){}[]~`!@#$%^&*?\"':;"

namespace ShapingEngine {

    namespace Helper {

        inline bool replace(std::string& str, const std::string& from, const std::string& to) {
            size_t start_pos = str.find(from);
            if (start_pos == std::string::npos)
                return false;
            str.replace(start_pos, from.length(), to);
            return true;
        }

        inline bool wreplace(std::wstring& str, const std::wstring& from, const std::wstring& to) {
            size_t start_pos = str.find(from);
            if (start_pos == std::wstring::npos)
                return false;
            str.replace(start_pos, from.length(), to);
            return true;
        }

        inline std::wstring widen(const std::string& utf8)
        {
            std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
            std::u16string utf16 = convert.from_bytes(utf8);
            std::wstring wstr(utf16.begin(), utf16.end());
            return wstr;
        }

        inline std::string narrow(const std::wstring& utf16) {
            std::u16string u16str(utf16.begin(), utf16.end());
            std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
            std::string utf8 = convert.to_bytes(u16str);
            return utf8;
        }

        inline bool contains_wchar_t(const wchar_t* text, wchar_t letter) {
            int i = 0;
            while (text[i] != '\0')
            {
                if (text[i] == letter)
                    return true;
                i++;
            }
            return false;
        }

        // Arabic letters range from U+0600 to U+06FF
        // (and U+FB50 to U+FDFF for Arabic Presentation Forms-A)
        // (and U+FE70 to U+FEFF for Arabic Presentation Forms-B)
        inline bool is_arabic_letter(wchar_t c, bool space = false, bool symbols = false) {
            int asciiValue = (int)c;

            if (asciiValue >= 1536 && asciiValue <= 1791
                || asciiValue >= 64336 && asciiValue <= 65023
                || asciiValue >= 65136 && asciiValue <= 65279 || asciiValue == ' ' && space
                || (symbols && contains_wchar_t(RENDER_SYMBOLS, asciiValue))) {
                return true;
            }

            return false;
        }

        inline bool is_arabic_vowel(wchar_t v) {
            // check if the character is a vowel
            int asciiValue = (int)v;
            if (asciiValue >= 1611 && asciiValue <= 1631)
                return true;
            return false;
        }
    }

    class Glyph;

    static std::map<int, Glyph> glyphs;

    class Glyph {
    public:
        wchar_t glyphInitial;
        wchar_t glyphMiddle;
        wchar_t glyphFinal;

        Glyph(std::wstring three_char_glyph) {
            this->glyphInitial = three_char_glyph[0];
            this->glyphMiddle = three_char_glyph[1];
            this->glyphFinal = three_char_glyph[2];
        }

        Glyph(wchar_t glyphInitial, wchar_t glyphMiddle, wchar_t glyphFinal) {
            this->glyphInitial = glyphInitial;
            this->glyphMiddle = glyphMiddle;
            this->glyphFinal = glyphFinal;
        }

        bool _initial() {
            return glyphInitial == ' ' ? false : true;
        }

        bool _middle() {
            return glyphMiddle == ' ' ? false : true;
        }

        bool _final() {
            return glyphFinal == ' ' ? false : true;
        }

        bool _initial_or_middle() {
            return _initial() || _middle();
        }

        bool _middle_or_final() {
            return _middle() || _final();
        }

        static void init_arabic() {
            if (glyphs.size() != 0)
                return;

            // ****** SPACES ARE IMPORTANT! DO NOT REMOVE!! ****** //

            glyphs.insert({ L'\u0640', Glyph(L"\u0640 \u0640") }); // ـ
            glyphs.insert({ L'\u0627', Glyph(L"  \uFE8E") }); // ا
            glyphs.insert({ L'\u0623', Glyph(L"  \uFE84") }); // أ
            glyphs.insert({ L'\u0625', Glyph(L"  \uFE88") }); // إ
            glyphs.insert({ L'\u0622', Glyph(L"  \uFE82") }); // آ
            glyphs.insert({ L'\u0628', Glyph(L"\uFE91\uFE92\uFE90") }); // ب
            glyphs.insert({ L'\u062A', Glyph(L"\uFE97\uFE98\uFE96") }); // ت
            glyphs.insert({ L'\u062B', Glyph(L"\uFE9B\uFE9C\uFE9A") }); // ث
            glyphs.insert({ L'\u062C', Glyph(L"\uFE9F\uFEA0\uFE9E") }); // ج
            glyphs.insert({ L'\u062D', Glyph(L"\uFEA3\uFEA4\uFEA2") }); // ح
            glyphs.insert({ L'\u062E', Glyph(L"\uFEA7\uFEA8\uFEA6") }); // خ
            glyphs.insert({ L'\u062F', Glyph(L"  \uFEAA") }); // د
            glyphs.insert({ L'\u0630', Glyph(L"  \uFEAC") }); // ذ
            glyphs.insert({ L'\u0631', Glyph(L"  \uFEAE") }); // ر
            glyphs.insert({ L'\u0632', Glyph(L"  \uFEB0") }); // ز
            glyphs.insert({ L'\u0633', Glyph(L"\uFEB3\uFEB4\uFEB2") }); // س
            glyphs.insert({ L'\u0634', Glyph(L"\uFEB7\uFEB8\uFEB6") }); // ش
            glyphs.insert({ L'\u0635', Glyph(L"\uFEBB\uFEBC\uFEBA") }); // ص
            glyphs.insert({ L'\u0636', Glyph(L"\uFEBF\uFEC0\uFEBE") }); // ض
            glyphs.insert({ L'\u0637', Glyph(L"\uFEC3\uFEC4\uFEC2") }); // ط
            glyphs.insert({ L'\u0638', Glyph(L"\uFEC7\uFEC8\uFEC6") }); // ظ
            glyphs.insert({ L'\u0639', Glyph(L"\uFECB\uFECC\uFECA") }); // ع
            glyphs.insert({ L'\u063A', Glyph(L"\uFECF\uFED0\uFECE") }); // غ
            glyphs.insert({ L'\u0641', Glyph(L"\uFED3\uFED4\uFED2") }); // ف
            glyphs.insert({ L'\u0642', Glyph(L"\uFED7\uFED8\uFED6") }); // ق
            glyphs.insert({ L'\u0643', Glyph(L"\uFEDB\uFEDC\uFEDA") }); // ك
            glyphs.insert({ L'\u0644', Glyph(L"\uFEDF\uFEE0\uFEDE") }); // ل
            glyphs.insert({ L'\u0645', Glyph(L"\uFEE3\uFEE4\uFEE2") }); // م
            glyphs.insert({ L'\u0646', Glyph(L"\uFEE7\uFEE8\uFEE6") }); // ن
            glyphs.insert({ L'\u0647', Glyph(L"\uFEEB\uFEEC\uFEEA") }); // ه
            glyphs.insert({ L'\u0648', Glyph(L"  \uFEEE") }); // و
            glyphs.insert({ L'\u064A', Glyph(L"\uFEF3\uFEF4\uFEF2") }); // ي
            glyphs.insert({ L'\u0626', Glyph(L"\uFE8B\uFE8C\uFE8A") }); // ئ
            glyphs.insert({ L'\u0649', Glyph(L"\u0649\u0649\uFEF0") }); // ى
            glyphs.insert({ L'\u0624', Glyph(L"  \uFE86") }); // ؤ
            glyphs.insert({ L'\u0629', Glyph(L"  \uFE94") }); // ة
            glyphs.insert({ L'\uFEFB', Glyph(L"  \uFEFC") }); // لا
            glyphs.insert({ L'\uFEF7', Glyph(L"  \uFEF8") }); // لأ
            glyphs.insert({ L'\uFEF9', Glyph(L"  \uFEFA") }); // لإ
            glyphs.insert({ L'\uFEF5', Glyph(L"  \uFED6") }); // لآ
        }
    };

    // Finds and reoders any arabic text in the wstring.
    inline void reorder_glyphs(std::wstring& t, bool symbols) {
        bool f = false;
        int ix = 0;
        for (int i = 0; i < t.length(); i++) {
            wchar_t l = t[i];
            if (Helper::is_arabic_letter(l, f, symbols) && !f) {
                f = true;
                ix = i;
            }
            if (!Helper::is_arabic_letter(l, true, symbols) && f) {
                f = false;
                int iy = i;
                while (t[ix] == ' ')
                    ix++;
                while (t[iy - 1] == ' ')
                    iy--;
                std::reverse(t.begin() + ix, t.begin() + iy);
            }
        }
        if (f) {
            while (t[ix] == ' ')
                ix++;
            std::reverse(t.begin() + ix, t.end());
        }
    }

    inline Glyph _glyph(int glyph) {
        auto pos = glyphs.find(glyph);
        if (pos != glyphs.end())
            return pos->second;
        else return Glyph(L"   ");
    }

    struct vowel_index {
        int index;
        int unicode;
    };

    // Finds and shapes any arabic text in the wstring and then returns it.
    // (Converts to arabic presentation forms A-B, Also Takes care of vowels in words).
    inline void shape_glyphs(std::wstring& t) {
        std::vector<vowel_index> vowels;
        for (int i = 0; i < t.length(); i++)
        {
            if (Helper::is_arabic_vowel(t[i]))
                vowels.push_back({ i, (int)t[i] });
        }
        wchar_t ww[2];
        for (int i = 1611; i <= 1631; i++)
        {
            ww[0] = (wchar_t)i;
            ww[1] = '\x0';
            while (Helper::wreplace(t, ww, L""));
        }
        std::wstring c = t;
        for (int i = 0; i < c.length(); i++) {
            if (i == 0) {
                auto g = _glyph(c[i]);
                if (g._initial() && _glyph(c[i + 1])._middle_or_final())
                    t[i] = g.glyphInitial;
            }
            else if (i == c.length() - 1) {
                auto g = _glyph(c[i]);
                if (g._final() && (_glyph(c[i - 1])._initial_or_middle()))
                    t[i] = g.glyphFinal;
            }
            else {
                auto g = _glyph(c[i]);
                if (g._middle() && (_glyph(c[i - 1])._initial()))
                {
                    t[i] = g.glyphFinal;
                    if ((_glyph(c[i + 1])._middle_or_final()))
                        t[i] = g.glyphMiddle;
                }
                else if (g._middle() && (_glyph(c[i + 1])._final()))
                {
                    t[i] = g.glyphInitial;
                    if ((_glyph(c[i - 1])._initial_or_middle()))
                        t[i] = g.glyphMiddle;
                }
                else if (g._final() && (_glyph(c[i - 1])._initial_or_middle()))
                    t[i] = g.glyphFinal;
                else if (g._initial() && (_glyph(c[i + 1])._middle_or_final()))
                    t[i] = g.glyphInitial;
            }
        }

        for (auto i : vowels) {
            ww[0] = (wchar_t)i.unicode;
            ww[1] = '\x0';
            t.insert(i.index, ww);
        }
        vowels.clear();
    }

    // Render a piece of text containing arabic text.
    // @param render_with_symbols renders arabic text while treating symbol characters like arabic letter.
    inline std::string render(std::wstring text, bool render_with_symbols = false) {
        Glyph::init_arabic();
        // *** For special characters like لا لأ لإ لآ
        while (Helper::wreplace(text, L"\u0644\u0627", L"\uFEFB")); // لا
        while (Helper::wreplace(text, L"\u0644\u0623", L"\uFEF7")); // لأ
        while (Helper::wreplace(text, L"\u0644\u0625", L"\uFEF9")); // لإ
        while (Helper::wreplace(text, L"\u0644\u0622", L"\uFEF5")); // لآ
        // ***************************************
        shape_glyphs(text);
        reorder_glyphs(text, render_with_symbols);
        return Helper::narrow(text);
    }
}