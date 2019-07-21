// From: http://codeflow.org/entries/2012/aug/02/easy-wireframe-display-with-barycentric-coordinates/

#extension GL_OES_standard_derivatives : enable

in lowp vec3 _barycentric;

float edgeFactor(){
    vec3 d = fwidth(_barycentric);
    vec3 a3 = smoothstep(vec3(0.0), d*1.5, _barycentric);
    return min(min(a3.x, a3.y), a3.z);
}

void main(){
    if(gl_FrontFacing){
        gl_FragColor = vec4(0.0, 0.0, 0.0, (1.0-edgeFactor())*0.95);
    }
    else{
        gl_FragColor = vec4(0.0, 0.0, 0.0, (1.0-edgeFactor())*0.7);
    }
}