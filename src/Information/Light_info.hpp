#pragma once

#include "../Vec3/Vec3.hpp"
#include "../Color/FColor.hpp"

struct Light_info{//光源についての情報
	//const R distance;//光源までの距離
	const Vec3 point;
    const R pdf;//サンプルポイントのpdf
	const FColor emission;//光の強さ
	//const Vec3 direction;//光源への方向
	const Vec3 normal;
	Light_info() = delete;
	//inline constexpr Light_info(const R d,const R p,const FColor &e,const Vec3 &_d,const Vec3 &n) : distance(d),pdf(p),emission(e),direction(_d.normalized()),normal(n.normalized()){};
	inline constexpr Light_info(const Vec3 &po,const R p,const FColor &e,const Vec3 &n) : point(po),pdf(p),emission(e),normal(n.normalized()){};
};
