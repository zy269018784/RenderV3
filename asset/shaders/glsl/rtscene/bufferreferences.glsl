#ifndef BUFFER_REFERENCES 
#define BUFFER_REFERENCES



layout(buffer_reference, scalar) buffer Vertices {vec4 v[]; };
layout(buffer_reference, scalar) buffer Indices {uint i[]; };


#endif