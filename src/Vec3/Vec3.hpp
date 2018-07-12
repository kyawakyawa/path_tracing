#pragma once

#include<iostream>
#include<cmath>
#define EPS (1.0 / 512.0)
#define EQ(a,b) (std::abs((a)-(b)) < EPS)//２つの実数が等しいか
#define INF (1e9)

typedef float R;


//https://qiita.com/Reputeless/items/96226cfe1282a014b147#5-%E3%83%A1%E3%83%B3%E3%83%90%E9%96%A2%E6%95%B0%E3%82%92%E4%BD%9C%E3%82%8D%E3%81%86
struct Vec3{
		R x;
		R y;
		R z;

		inline Vec3() = default;//: x(0.0),y(0.0),z(0.0){};
		inline constexpr Vec3(const R _x,const R _y,const R _z): x(_x),y(_y),z(_z){};

		inline constexpr Vec3 operator +() const{//　単項+   ex +a
			return *this;
		}
		inline constexpr Vec3 operator -() const{//  単項-   ex -a
			return{ -x,-y,-z};
		}

		inline constexpr Vec3 operator + (const Vec3 &obj) const{// 2項+ ex a + b
			return Vec3(this->x + obj.x,this->y + obj.y,this->z + obj.z);
		}
		inline constexpr Vec3 operator - (const Vec3 &obj) const{// 2項- ex a - b
			return Vec3(this->x - obj.x,this->y - obj.y,this->z - obj.z);
		}

		inline constexpr R operator * (const Vec3 &obj) const{// 内積 
			return this->x * obj.x + this->y * obj.y + this->z * obj.z;
		}


		inline constexpr Vec3 operator * (const R a) const{//スカラー積（後ろがスカラー)
			return Vec3(this->x * a,this->y * a,this->z * a);
		}
		inline constexpr Vec3 operator / (const R a) const{//スカラー除
			return Vec3(this->x / a,this->y / a,this->z / a);
		}

		inline Vec3& operator +=(const Vec3& obj){
			x += obj.x;
			y += obj.y;
			z += obj.z;
			return *this;
		}
		inline Vec3& operator -=(const Vec3& obj){
			x -= obj.x;
			y -= obj.y;
			z -= obj.z;
			return *this;
		}
		inline Vec3& operator *=(const R a){
			x *= a;
			y *= a;
			z *= a;
			return *this;
		}
		inline Vec3& operator /=(const R a){
			x /= a;
			y /= a;
			z /= a;
			return *this;
		}

		inline constexpr bool operator ==(const Vec3 &obj) const{
			return EQ(this->x,obj.x) && EQ(this->y,obj.y) && EQ(this->z,obj.z);
		}

	 	inline R abs() const{//gccならconstexprにしても良い
			return std::sqrt(x * x + y * y + z * z);
		}
		inline constexpr R abs_square() const{
			return x * x + y * y + z * z;
		}
		inline Vec3 normalized() const {//正規化（ベクトルの長さを1にする)ベクトル
			const R l = 1.0 / std::sqrt(x * x + y * y + z * z);//逆数であることを明示した方が早い？
			return { x * l,y * l , z * l};
		}
};

inline constexpr Vec3 operator *(R s,const Vec3& v){//スカラー*ベクトル
	return {s * v.x,s * v.y,s * v.z};
}

inline constexpr Vec3 cross(const Vec3 &v1,const Vec3 &v2){
	return Vec3(
		(v1.y * v2.z) - (v1.z * v2.y),
		(v1.z * v2.x) - (v1.x * v2.z),
		(v1.x * v2.y) - (v1.y * v2.x));
}

inline Vec3 rotate(const Vec3 &p,const Vec3 &rin,const R theta){
	const Vec3 r = rin.normalized();
	const R s = std::sin(theta * 0.5),c = std::cos(theta * 0.5);

	const R w1 = c;
	const R x1 = -r.x * s;
	const R y1 = -r.y * s;
	const R z1 = -r.z * s;

	const R w0 = x1 * p.x + y1 * p.y + z1 * p.z;
	const R x0 = w1 * p.x - y1 * p.z + z1 * p.y;
	const R y0 = w1 * p.y - z1 * p.x + x1 * p.z;
	const R z0 = w1 * p.z - x1 * p.y + y1 * p.x;

	return Vec3(w0 * x1 + w1 * x0 + y0 * z1 - z0 * y1,
				w0 * y1 + w1 * y0 + z0 * x1 - x0 * z1,
				w0 * z1 + w1 * z0 + x0 * y1 - y0 * x1);
}
	
template <class Char> // 出力ストリーム
inline std::basic_ostream<Char>& operator <<(std::basic_ostream<Char>& os, const Vec3& v){
    return os << Char('(') << v.x << Char(',') <<v.y << Char(',') << v.z << Char(')');
}

template <class Char> // 入力ストリーム (x,y,z) と入力する
inline std::basic_istream<Char>& operator >>(std::basic_istream<Char>& is, Vec3& v){
    Char unused;
    return is >> unused >> v.x >> unused >> v.y >> unused >> v.z >> unused;
}
