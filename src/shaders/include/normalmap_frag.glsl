
vec3 GetNormal () {
	vec3 result;
	result = texture(normalSampler fs_in.texCoord).rgb; // Get normal from normal_map
	result = normalize((result * 2.0) - 1.0); // Map the Normal to [-1, 1]
	result = normalize(fs_in.tbn * result); // Transform the normal into worldSpace
	return normal;
}
