#pragma once

#include "../Vec3/Vec3.hpp"
#include "../Color/FColor.hpp"

struct Light_info{//光源についての情報
	const Vec3 point;
    const R pdf;//サンプルポイントのpdf
	const FColor emission;//光の強さ
	const Vec3 normal;
	Light_info() = delete;
	inline constexpr Light_info(const Vec3 &po,const R p,const FColor &e,const Vec3 &n) : point(po),pdf(p),emission(e),normal(n.normalized()){};
};
