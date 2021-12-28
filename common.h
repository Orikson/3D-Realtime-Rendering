#ifndef _COMMON_H
#define _COMMON_H

/*=======USEFUL MATH CONSTANTS=======*/
#define PI 3.141592653589793238462643383279502884197169399375105820974944592307816


/*=======STANDARD TEMPLATE LIBRARY=======*/
#include <iostream>
#include <string>
#include <cstdlib>
#include <memory>
#include <ctime>
#include <vector>
#include <math.h>
#include <sstream>
#include <time.h>
#include <tuple>
#include <limits>
#include <iomanip>
extern "C" {
    #include <unistd.h>
}

using namespace std;
namespace patch {
    template <typename T> string to_string(const T& n) {
        ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}

template<typename Base, typename T> inline bool instanceof(const T*) {
    return std::is_base_of<Base, T>::value;
}


/*=======GLU/GLEW=======*/
#include <GL/glew.h>
#include <GL/glu.h>
#include <GL/gl.h>


/*=======SDL2=======*/
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>


/*=======CLASS DEFINITIONS=======*/
#include "Engine/Graphics/graphics.h"

#endif