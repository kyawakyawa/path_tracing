#pragma once
#include<cstdio>
#include<algorithm>

typedef float R;

struct FColor{
    R red;
    R green;
    R blue;

    FColor() = default;
    inline constexpr FColor(R r,R g,R b): red(r),green(g),blue(b) {};

    inline void set(R r,R g,R b){//セッター
        red = r;green = g;blue = b;
    }

    inline void print255() const{//出力
        R r = red;
        R g = green;
        R b = blue;
        r = std::max((R)0.0,r);r = std::min((R)1.0,r);
        g = std::max((R)0.0,g);g = std::min((R)1.0,g);
        b = std::max((R)0.0,b);b = std::min((R)1.0,b);
        int ir = std::pow(r,1.0 / 2.2) * 255 + 0.5;
        int ig = std::pow(g,1.0 / 2.2) * 255 + 0.5;
        int ib = std::pow(b,1.0 / 2.2) * 255 + 0.5;

        printf("%d %d %d\n",ir,ig,ib);
    }

    inline constexpr FColor operator + (const FColor &obj) const{// 2項+ ex a + b
        return FColor(this->red + obj.red,this->green + obj.green,this->blue + obj.blue);
    }
    inline constexpr FColor operator - (const FColor &obj) const{// 2項- ex a - b
        return FColor(this->red- obj.red,this->green - obj.green,this->blue - obj.blue);
    }
    inline constexpr FColor operator * (const FColor &obj) const{// 要素同士の掛け算
        return FColor(this->red * obj.red, this->green * obj.green , this->blue * obj.blue);
    }

    inline constexpr FColor operator * (const R r) const{//要素同士の掛け算
        return FColor(red * r,green * r,blue * r);
    }

    inline FColor& operator +=(const FColor& obj){
        red += obj.red;
        green += obj.green;
        blue += obj.blue;
        return *this;
    }
    inline FColor& operator -=(const FColor& obj){
        red -= obj.red;
        green += obj.green;
        blue += obj.blue;
        return *this;
    }
    inline FColor& operator *=(const FColor& obj){
        red *= obj.red;
        green *= obj.green;
        blue *= obj.blue;
        return *this;
    }
    inline FColor& operator *=(const R r){
        red *= r;
        green *= r;
        blue *= r;
        return *this;
    }
 	inline R abs() const{//gccならconstexprにしても良い
		return std::sqrt(red * red + green * green + blue * blue);
	}
};

inline constexpr FColor operator *(const R s,const FColor& v){//スカラー*ベクトル
	return {s * v.red,s * v.green,s * v.blue};
}

template <class Char> // 出力ストリーム
inline std::basic_ostream<Char>& operator <<(std::basic_ostream<Char>& os, const FColor& v){
    return os << Char('(') << v.red << Char(',') <<v.green << Char(',') << v.blue << Char(')');
}

template <class Char> // 入力ストリーム (x,y,z) と入力する
inline std::basic_istream<Char>& operator >>(std::basic_istream<Char>& is, FColor& v){
    Char unused;
    return is >> unused >> v.red >> unused >> v.green >> unused >> v.blue >> unused;
}
