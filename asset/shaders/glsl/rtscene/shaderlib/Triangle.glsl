
void CalcTriangle(Triangle tri)
{
	// 顶点
	vec3 p0 = tri.vertices[0].pos;
	vec3 p1 = tri.vertices[1].pos;
	vec3 p2 = tri.vertices[2].pos;
	vec3 dp02 = p0 - p2;
	vec3 dp12 = p1 - p2;
	
	// uv
	vec2 uv0 = tri.vertices[0].uv;
	vec2 uv1 = tri.vertices[1].uv;
	vec2 uv2 = tri.vertices[2].uv;
	vec2 duv02 = uv0 - uv2;
	vec2 duv12 = uv1 - uv2;
	
	// 法线  
	vec3 n0 = tri.vertices[0].normal; 
	vec3 n1 = tri.vertices[1].normal;
	vec3 n2 = tri.vertices[2].normal;
	vec3 dn01 = n0 - n2;
    vec3 dn12 = n1 - n2;
	
	// 切线
	vec3 ss0;
	vec3 ss1;
	vec3 ss2;
	
	// 重心
	float b0;
	float b1;
	float b2;
	
	 // 几何信息
	vec3 g_n;
	vec3 g_dpdu, g_dpdv;
	vec3 g_dndu, g_dndv;
	
	// 着色信息
	vec3 s_n;
	vec3 s_dpdu, s_dpdv;
	vec3 s_dndu, s_dndv;
	
	// 几何法线
    g_n = normalize(cross(dp02, dp12));
	
	// 着色法线
    s_n = b0 * n0 + b1 * n1 + b2 * n2;
	
	float determinant = DifferenceOfProducts(duv02[0], duv12[1], duv02[1], duv12[0]);
	bool degenerateUV = abs(determinant) < 1e-9f;
	if (!degenerateUV)
	{
		// Compute triangle $\dpdu$ and $\dpdv$ via matrix inversion
		float invdet = 1 / determinant;
		g_dpdu = DifferenceOfProducts(duv12[1], dp02, duv02[1], dp12) * invdet;
		g_dpdv = DifferenceOfProducts(duv02[0], dp12, duv12[0], dp02) * invdet;
	}
	
    if (degenerateUV || LengthSquared(cross(g_dpdu, g_dpdv)) == 0) 
	{
		vec3 ng = cross(p2 - p0, p1 - p0);
		if (LengthSquared(ng) == 0) 
		{
			ng = vec3(cross(dvec3(p2 - p0), dvec3(p1 - p0)));
		}
		CoordinateSystem(normalize(ng), &g_dpdu, &g_dpdv);
	}
	
	// 如果网格提供了切线, 需要重新计算偏导
	s_dpdu = g_dpdu;
	s_dpdv = g_dpdv;

	// 几何法线不变, 所以偏导为0
	g_dndu = vec3(0.f);
	g_dndv = vec3(0.f);
	
    // 如果网格提供了法线, 需要重新计算偏导
	s_dndu = vec3(0.f);
	s_dndv = vec3(0.f);
	
	vec3 ss = g_dpdu;
	if (has_tangent)
	{
		ss = b0 * ss0 + b1 * ss1 + b2 * ss2;
		if (LengthSquared(ss) == 0)
			ss = g_dpdu;
	}
	
	// 计算副切线
	vec3 ts = cross(s_n, ss);
	// 调整切线
	if (LengthSquared(ts) > 0)
		ss = cross(ts, s_n);
	else	
		CoordinateSystem(s_n, &ss, &ts);
		
	vec3 dndu, dndv;	
	if (degenerateUV) 
	{
		vec3 dn = cross(vec3(n2 - n0), vec3(n1 - n0));
		if (LengthSquared(dn) == 0)
			dndu = dndv = vec3(0, 0, 0);
		else {
			vec3 dnu, dnv;
			CoordinateSystem(dn, &dnu, &dnv);
			dndu = vec3(dnu);
			dndv = vec3(dnv);
		}
	}
	else 
	{
		Float invDet = 1 / determinant;
		dndu = DifferenceOfProducts(duv12[1], dn01, duv02[1], dn12) * invDet;
		dndv = DifferenceOfProducts(duv02[0], dn12, duv12[0], dn01) * invDet;
	}
	
	// 计算着色法线
	s_n = LengthSquared(s_n) > 0 ? Normalize(s_n) : g_n;
	s_dpdu = ss;
    s_dpdv = ts;
	s_dndu = dndu;
	s_dndv = dndv;

}

