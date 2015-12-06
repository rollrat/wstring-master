# wstring-master (Simple String Implements)

This library provides an easy-to-use string analysis method for **Windows System**. We are promoting the maximum optimization of all methods.

### Targeted Compiler
All of functions that are included in this library, has a wchar_t type, this type is, will vary in size by the compiler, 
in other platforms except the windows, you will need to change some.

This project has been developed in visual-studio-2013.

### Necessary improvement part In WString
In `WString.h`, using class `StringReferencePutWith` is used for direct pointer passed in the constructor.
The reason for using this approach is to avoid duplicate allocation to be made in case the string 
pointer conversion when using this class from another class.  It is used in order to completely attributed 
the string to the class, which means that it is released in the destructor. This code is example of this.
``` c++
WString getMark()
{
    wchar_t* dest = new wchar_t[10];
    memcpy(dest, L"rollrat", 7 * sizeof(wchar_t));
    StringReferencePutWith with;
    return WString(dest, 7, with);
}
```
Also, in the constructor, there are two ambiguous expression, one is a `wchar_t *, size_t and const wchar_t *, size_t`, 
the other one is a `std::wstring& and const std::wstring&`.
In each former, the pointer in the destructor is released, but the latter will not be released.

The most important feature, some of the functions that are included in the standard library 
is that is directly implemented, which is to replace the late functions of purely visual studio.
It may be slower than the function implemented by the SIMD but shows a high-speed 
performance several times more than the visual studio functions at least.
Especially wcslen function, showed about 10 times faster than the existing function.
It was confirmed that it has not been optimized in assembly.
Especially functions related wide-string, there were many functions that are not optimized.

---

## Method Details
### Constructor
We provide some of convenient constructor input. The following is all the constructors provided.
##### Null String
``` c++
WString();
```
This generates a null string. It can be confirmed that the null string by `Null` function.
##### Constant String
``` c++
WString(const char*);
WString(const char*, size_t);
WString(const wchar_t*);
WString(const wchar_t*, size_t);
```
The list of overloading copy the string (by length). Especially in the case of `char`, change internally mbcs to wide-cs format.
##### Non-constans String
``` c++
WString(wchar_t*, size_t);
WString(wchar_t*, size_t, StringReferencePutWith);
```
This part is ambiguous part by the above brief introduction. The first expression don't released in the destructor, but released in second expression.
##### Character
``` c++
WString(char);
WString(char, size_t);
WString(wchar_t);
WString(wchar_t, size_t);
```
If size_t parameter is blank, copy the character only, but if are filled, replicate the character with a length. This is example.
``` c++
WString(L'a', 5); // -> L"aaaaa"
```
##### Number
``` c++
WString(int);
WString(long int);
WString(long long int);
WString(unsigned int);
WString(unsigned long int);
WString(unsigned long long int);
WString(float);
WString(double);
WString(long double);
```
`signed` and `short` is not supported.
##### STL string
``` c++
WString(std::wstring&);
WString(const std::string&);
WString(const std::wstring&);
```
The first syntax generate a pointer of wstring, but the remaining two copies the string.
### Length, Empty, Full, Null
`Length` is brought the length of string, `Empty` check the length is 0, `Full` make sure that the length of the string is not 0, and `Null` chechk string is generated. Here is the example.
``` c++
WString().Null(); // true
WString("").Null(); // false
WString("").Empty(); // true
WString("rollrat").Empty(); // false
WString("").Full(); // false
WString("rollrat").Full(); // true
```
### Reference
You can get a pointer to string using this function. This function, when using `wchar_t *` directed only, should be used. This same as the **c_str** function of stl string class.
### Append
``` c++
WString("rat").Append("roll");
// -> rollrat
```
This function concaterate source with class-ptr. In other words, it be process as push-front.
### Concat (*static*)
``` c++
WString::Concat(const WString&, const WString&);
WString::Concat(const WString&, const WString&, const WString&);
WString::Concat(const WString&, const WString&, const WString&, const WString&);

WString::Concat("concat", " ", "test");
// -> concat test
```
This function concaterate class-ptr with source. It concaterate maximum four of the string for the order of the argument.
### CompareTo, Comparer(*static*), Equal
``` c++
WString("1234").CompareTo("1235"); // -> -1

SortWith(wstr-array, index-array, WString::Comparer); // example

WString("rollrat").Equal("rollrat); // true

// operator==(const WString&)
WString("rollrat") == "rollrat"; // true
// operator==(const wchar_t*)
WString("rollrat") == L"rollrat"; // true
```
The functions of compare strings, such as *wcscmp*. When using a raw string, it is necessary to reduce the waste of resources during operation using the Unicode strings.
### First, Last
``` c++
WString("rollrat").First(4); // -> L'r'
WString("rollrat").Last(1); // -> L'a'
```
`First` returns the character of the position from the front and, `Last` returns the character of the position from the back.
These functions check the position. So, these functions are safety, buf slow.
### FindFirst, FindLast
``` c++
WString("rollrat rollrat").FindFirst("rat"); // -> 4
WString("rollrat rollrat").FindFirst("rat", 5); // -> 12
WString("rollrat rollrat").FindLast("rat"); // -> 12
WString("rollrat rollrat").FindLast("rat",1); // -> 4
WString("abcdefg").FindFirst(L'c'); // -> 2
```
`FindFirst` function returns the position of the char or string the first match from input position, and `FindLast` function returnsthe position of the char or string the last match from *length - input position*. In other words, `FindFirst` is front search function, and `FindLast` is back search function. Position start from in the first and last.
### Contains
``` c++
bool Contains(wstr, [ignore case = false]);
```
``` c++
WString("contains test. by rollrat").Contains("test"); // -> true
```
This function check whether it contains input string.
### Substring, SubstringReverse
``` c++
WString Substring(size_t starts); // len = m_length - starts
WString Substring(size_t starts, size_t len);
WString SubstringReverse(size_t starts); // len = m_length - starts
WString SubstringReverse(size_t starts, size_t len);
```
``` c++
WString("substring test").Substring(3); // -> "string test"
WString("substring test").Substring(10, 4); // -> "test"
WString("substring test").SubstringReverse(5); // -> "substring"
WString("substring test").SubstringReverse(5,6); // -> "string"
```
`Substring` gets the string from starts position cut by len, and `SubstringReverse` Gets a string from behind from starts position cut by len.
### TrimStartPos, TrimEndPos
``` c++
size_t TrimStartPos(); // space
size_t TrimStartPos(wchar_t);
size_t TrimEndPos(); // space
size_t TrimEndPos(wchar_t);
```
``` c++
WString("   rollrat").TrimStartPos(); // -> 3
WString("rollrat   ").TrimEndPos(); // -> 6
WString("wwwrollrat").TrimStartPos(L'w'); // -> 3
WString("rollratwww").TrimEndPos(L'w'); // -> 6
```
These functions get the posistion where the characters overlap ends.
### TrimStart, TrimEnd, Trim
``` c++
WString TrimStart();
WString TrimStart(wchar_t);
WString TrimEnd();
WString TrimEnd(wchar_t);
WString Trim();
WString Trim(wchar_t);
WString Trim(wstr);
```
``` c++
WString("   rollrat  ").TrimStart(); // -> "rollrat  "
WString("   rollrat  ").TrimEnd(); // -> "   rollrat"
WString("   rollrat  ").Trim(); // -> "rollrat"
WString("wwwrollratwww").Trim(); // -> "rollrat"
WString("ssskkkrrrkkkttt").Trim("kkk"); // -> "sssrrrttt"
```
These functions returns string what is deleted duplicate characters. `TrimStart` is removed from the starting position, `TrimEnd` is deleted from the end position, and `Trim` remove both. But, the entered type is wide-string, delete all the strings included.
### Count
``` c++
WString("abc acb abc").Count("abc"); // -> 2
WString("abcabcabc").Count(L'a'); // -> 3
```
This function count the number of the included characters or string.
### Split, SplitReverse
``` c++
SplitsArray Split(wstr, [max] = SIZE_MAX);
SplitsArray SplitReverse(wstr, [max] = SIZE_MAX);
```
``` c++
WString wstr(L"1|splits|234|splits|56|splits|78");
WString::SplitsArray splits = wstr.Split(L"|splits|", 3);
splits.Each([] (const WString* refer) { std::wcout << *refer <<  L' '; });
// -> "1 234 56 "
WString::SplitsArray splits = wstr.SplitReverse(L"|splits|", 3);
splits.Each([] (const WString* refer) { std::wcout << *refer <<  L' '; });
// -> "78 56 234 "
```
This function bring an array of cutting string based on the wstr only max. `SplitReverse` function cut from the last position.
### Between, Betweens
``` c++
WString Between(left, right, [starts] = 0);
SplitsArray Betweens(left, right, [starts] = 0);
```
``` c++
WString("{{{get}}}").Between("{", "}"); // -> "{{get"
WString("{{{get}}}").Between("{", "}",2); // -> "get"
WString("a{b{c}d{e}f}g").Betweens("{", "}"); // -> "b{c", "e"
```
This function bring the strings that between the left and the right string.
### ToLower, ToUpper, Cpaitalize, Title
``` c++
WString("roLLraT").ToLower(); // -> "rollrat"
WString("roLLraT").ToUpper(); // -> "ROLLRAT"
WString("the person").Capitalize(); // -> "The person"
WString("the person").Tile(); // -> "The Person"
```
`ToLower` lowercase all characters, `ToUpper` uppercase all characters, `Capitalize` capitalize the first character of the string, `Title` first letter of every word in capital letters.
### StartsWith, EndsWith
``` c++
WString StartsWith(wstr, [starts] = 0);
WString EndsWith(wstr, [ends] = 0);
```
``` c++
WString("it is startswith").StartsWith("it"); // true
WString("it is startswith").StartsWith("is", 3); // true
WString("it is endswith").EndsWith("endswith"); // true
WString("it is endswith").EndsWith("ends", 4); // true
```
`StartsWith` checks whether or not match from the first position of the string, `EndsWith` checks whether match from the last position in the string. `starts` and `ends`  is the first and last position of each of the search.
### PadLeft, PadRight, PadMiddle
``` c++
WString PadLeft(len, [pad] = L' ');
WString PadRight(len, [pad] = L' ');
WString PadMiddle(len, [pad = L' ', [lefts = true]]);
```
``` c++
WString("123").PadLeft(4); // -> " 123"
WString("123").PadLeft(2); // -> "123"
WString("123").PadRight(4); // -> "123 "
WString("123").PadRight(2); // -> "123"
WString("123").PadLeft(4,L'k'); // -> "k123"
WString("123").PadRight(4,L'k'); // -> "123k"
WString("123").PadMiddle(6); // -> "  123 "
WString("123").PadMiddle(6,L'k',false); // -> "k123kk"
```
This function set string to each right and left ends, and fill the rest of the space by the pad. If the length is less than the length of the string, existing string is returned. The `PadMiddle` function sorts the strings in the middle. If `lefts` is true, string are aligned concentrated on the left.
### InsertLeft, InsertRight
``` c++
WString("1234567890").InsertLeft(3, L','); // -> 123,456,789,0
WString("1234567890").InsertRight(3, L','); // -> 1,234,567,890
WString("0123").InsertLeft(2, "ro"); // -> 01ro23
WString("01234").InsertRight(2, "ro"); // -> 0ro12ro34
```
Each divide the right or the left by `number`, and get a string that the character or string inserted in the between.
### Replace
``` c++
WString Replace(src, dest, [max] = SIZE_MAX);
```
``` c++
WString("rollratrollratrollrat").Replace("rollrat", "ok"); // -> "okokok"
WString("rollratrollratrollrat").Replace("rollrat", "ok", 1); // -> "okrollratrollrat"
```
Replace the src to dest as max.
### Remove, RemoveReverse
``` c++
WString Remove(len); // == Substring(0, len);
WString Remove(starts, len);
WString RemoveReverse(len); // == SubstringReverse(0, len);
WString RemoveReverse(starts, len); // == Remove(this_len - starts - len, len)
```
``` c++
WString("rollrat lab software").Remove(7); // -> "rollrat"
WString("rollrat lab software").Remove(8, 4); // -> "rollrat software"
WString("rollrat lab software").RemoveReverse(4); // -> "ware"
WString("rollrat lab software").RemoveReverse(8, 4); // -> "rollrat software"
```
This function sets the string that delete by length from starts.
### Insert
``` c++
WString Insert(starts, wstr, [len] = wcslen(wstr));
```
``` c++
WString("rrat").Insert(1, "oll"); // -> "rollrat"
WString("rrat").Insert(1, "ollrat", 3); // -> "rollrat"
```
This function sets a string to the added wstr by len in starts position.
### Repeat
``` c++
WString("rollrat").Repeat(3); // -> "rollratrollratrollrat"
```
Repeat string.
### Reverse
``` c++
WString("tarllor").Reverse(); // -> "rollrat"
```
Reverse string.
### Slice
``` c++
WString Slice(first, last);
WString Slice(skip); // == Slice(skip, -(int)skip);
```
``` c++
WString("[rollrat]").Slice(1, 7); // -> "rollrat"
WString("[rollrat]").Slice(1, -1); // -> "rollrat"
WString("[rollrat]").Slice(1); // -> "rollrat"
```
This function sets a string of `last` from the `first` location. If last is less equal than 0, by the end of the string to 0.
### Slicing
``` c++
WString Slicing(jmp, [starts = 0, [len = 1, [remain = true]]]);
```
``` c++
WString("01234567").Slicing(1); // -> "0246"
WString("%0123456").Slicing(1,1); // -> "0246"
WString("%01%23%45%67").Slicing(1,1,2); // -> "01234567"
WString("%01%23%45%6789").Slicing(1,1,2); // -> "012345679"
WString("%01%23%45%6789").Slicing(1,1,2,false); // -> "01234567"
```
`Slicing` function gets string by len moved by `jmp` times from starts. If `remain` is true, and gets also remain part.
### LineSplit (size_t, ...)
``` c++
Lines LineSplit(size_t len, [wstr front, [wstr end]]); // none-of olny end
```
``` c++
WString(L"0123456789abcdefghijklmnop").LineSplit(10, L" ->> ", L" <<- ").Each(
[&] (const WString* re) {
	std::wcout << *re << std::endl;
});
```
```
 ->> 0123456789 <<-
 ->> abcdefghij <<-
 ->> klmnop     <<- // added space
```
This function is that cut the string with len and set the string on fornt if `front` exist, and `end` to end of the string if exist. If the `end` is entered, and then add a space between the `end` and the string at the end of the line.
### LineSplit ([bool])
``` c++
Lines LineSplit([last] = false);
```
``` c++
WString(L"012\r\n345678\n9abcde\n").LineSplit().Each(
[&] (const WString* re) {
	std::wcout << *re << std::endl;
});
```
```
012
345678
9abcde
```
if last is true
```
012
345678
9abcde

```
This function cut a string as a boundary \r\n or \n. If `last` is true, if the last character is newline character, add newline string to returning array.
### LineBreak
``` c++
WString LineBreak(size_t len); // non-array version of LineSplit(len)
```
``` c++
WString("rollratrollrat").LineBreak(4); // -> "roll\r\nratr\r\nollr\r\nat"
```
This function gets a string line break by len. The newline character is not added to the last line.
### Hash
``` c++
uint64_t Hash(uint64_t seed = 0x8538dcfb7617fe9f);
```
This function sets a hash to seed. Built-in seed is a prime number.
### IsNumeric, IsHexDigit
``` c++
WString("0123456").IsNumeric(); // -> true
WString("0123456.e+3").IsNumeric(); // -> true
WString("0123456e+3").IsNumeric(); // -> true
WString("0123E13").IsHexDigit(); // -> true
WString("0x01234").IsHexDigit(); // -> true
```
Check if numeric or hex digit.
### ToHexDigit, ToChar, ToLongLong, ToULongLong, ToLong, ToULong, ToInteger, ToUInteger, ToShort, ToUShort, ToLongDouble, ToDouble, ToFloat

String to each formatting number.
### ToArray, ToAnsi

String to wchar_t*, char*. This is not pointer to m_ptr.
### ToUtf8
``` c++
Utf8Array ToUtf8([file_bom] = false);
```
Create a format of the utf8 bytes array. If `file_bom` is true, and utf8 file bom is inserted.
### Swap
``` c++
void Swap(WString&);
```
Swap m_ptr, m_last, m_length with source.
### Clone
``` c++
void Clone(WString&);
WString Clone();
```
Former, called class be a clone of the input string, latter, create a clone.

## WStringBuffer, WStringBuilder
We provide one of the class easily transformable into wstring. This implement will used in largely text append routine. 
Please refer the example enclosed.

## Similar functions in other languages
[Right, `Substring`] [Left, `SubstringReverse`] [Mid, `Substring` `Slice`] [RSet, `PadRight`] [LSet, `PadLeft`] [RTrim, `TrimStart`] [LTrim, `TrimEnd`] [IndexOf, `FindFirst`, FirstContains] [LastIndexfOf, `FindLast`, LastContains] [InStr, `Contains`]

## In development...
Comments written in English.

## License
wstring-mater is licensed under the [MIT License](LICENSE).
