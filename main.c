#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

const int WIDTH  = 500;
const int HEIGHT = 500;
const int FOCAL_LENGHT = 50;
const int SAMPLE_RATE = 1;
const int MAX_STEPS = 1;

typedef struct {
    double x;
    double y;
    double z;
} vect3;

typedef struct {
    float x;
    float y;
    float z;
} vect3_f;

typedef struct {
    int x;
    int y;
    int z;
} vect3_i;

typedef struct {
    vect3 base_color;
    vect3 reflectivity;
    double roughness;
} material;

typedef struct {
    vect3 pos;
    double radius;
    material material;
} sphere;

typedef struct {
    bool collided;
    vect3 point;
    double dist;
    vect3 normal;
    sphere object;
} intersection;


double mod(vect3 vec) {
    return sqrt(pow(vec.x, 2) + pow(vec.y, 2) + pow(vec.z, 2));
}

vect3 norm(vect3 vec) {
    double l = mod(vec);
    return (vect3){vec.x/l, vec.y/l, vec.z/l};
}

vect3 mult(vect3 vec, int a) { 
    return (vect3) {vec.x*a, vec.y*a, vec.z*a};
}

vect3 mult_2(vect3 vec1, vect3 vec2) { 
    return (vect3) {vec1.x*vec2.x, vec1.y*vec2.y, vec1.z*vec2.z};
}

vect3 sub(vect3 vec1, vect3 vec2) {
    return (vect3) {vec1.x-vec2.x, vec1.y-vec2.y, vec1.z-vec2.z};
}

vect3 add(vect3 vec1, vect3 vec2) {
    return (vect3) {vec1.x+vec2.x, vec1.y+vec2.y, vec1.z+vec2.z};
}

double dot(vect3 vec1, vect3 vec2) {
    return vec1.x*vec2.x + vec1.y*vec2.y + vec1.z*vec2.z;
}

vect3 gen_rand() {
    return (vect3) {(double)(rand()/RAND_MAX), (double)(rand()/RAND_MAX), (double)(rand()/RAND_MAX)};
}

vect3 reflect(vect3 ray_direction, vect3 normal) {
    double len = dot(ray_direction, normal) * 2;
    return sub(ray_direction, mult(normal, len));
}

intersection sphere_intersection(vect3 origin, vect3 ray_direction, sphere spheres[], unsigned int sphere_count) {
    bool collided = false;
    intersection* intersections = (intersection*) malloc((sphere_count+1)*sizeof(intersection));
    // set last element to be the fallback in case there is no intersection
    intersection false_intersection;
    false_intersection.collided = false;
    intersections[sphere_count] = false_intersection; 
    int closest_object_index = sphere_count;
    double closest_intersection = 0;


    for (unsigned int i = 0; i < sphere_count; ++i) {
        vect3 ray_sphere = sub(spheres[i].pos, origin);
        double dist_sphere = mod(ray_sphere);
        double dist_hit = dot(ray_sphere, ray_direction);
        double dist_sphere_hit = sqrt(fabs(pow(dist_sphere, 2) - pow(dist_hit, 2) ));

        double dist_intersection = dist_hit - sqrt(fabs(pow(spheres[i].radius, 2) - pow(dist_sphere_hit, 2)));
        vect3 point = add(origin, mult(ray_direction, dist_intersection));
        vect3 normal = norm(sub(point, spheres[i].pos)); 
        normal = norm(add(normal, mult(gen_rand(), spheres[i].material.roughness)));

        intersections[i] = (intersection) {
            false,
            point,
            dist_intersection,
            normal,
            spheres[i]
        };
        if (dist_hit > 0 && dist_sphere_hit < spheres[i].radius) {
            collided |= true;
            if (closest_intersection == 0 || dist_intersection < closest_intersection) {
                closest_intersection = dist_intersection;
                closest_object_index = i;
            }
            intersections[i].collided = true;
        }
    }
    intersection result_intersection = intersections[closest_object_index];
    free(intersections);
    return result_intersection;
}

vect3 draw(vect3 origin, vect3 ray_direction, sphere spheres[], unsigned int sphere_count, int steps) {
    intersection intersect = sphere_intersection(origin, ray_direction, spheres, sphere_count);
    vect3 result = (vect3) {0,0,0}; // base background color

    if (intersect.collided && steps > 0) {
        vect3 reflect_origin = intersect.point;
        vect3 reflect_direction = reflect(ray_direction, intersect.normal);
        
        vect3 extra = mult_2(
                    draw(reflect_origin, reflect_direction, spheres, sphere_count, steps-1),
                    intersect.object.material.reflectivity
                );
        result = add(intersect.object.material.base_color, extra);
    }
    return result;
}

bool ppm_save(vect3_i pixmap[WIDTH][HEIGHT], const char* fname) {
    FILE* file = fopen(fname, "wb");
    if (!file) {
        return false;
    }
    fprintf(file, "P3\n%d %d\n255\n", HEIGHT, WIDTH);
    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            fprintf(file, "%d %d %d\n", pixmap[i][j].x, pixmap[i][j].y, pixmap[i][j].z);
        }
    }
    return true;

}

int main() {
    srand((unsigned int)time(NULL));

    // init objects
    sphere spheres[5];
    spheres[0] = (sphere) {
        (vect3) {0,20,40},
        30,
        (material) {
            (vect3) {255,0,0},
            (vect3) {1,1,1},
            0
        }
    };
    spheres[1] = (sphere) {
        (vect3) {-10,0,20},
        10,
        (material) {
            (vect3) {0,100,0},
            (vect3) {1,1,1},
            2
        }
    };
    spheres[2] = (sphere) {
        (vect3) {40,-50,60},
        10,
        (material) {
            (vect3) {200,200,200},
            (vect3) {1,1,1},
            0.4
        }
    };
    spheres[3] = (sphere) {
        (vect3) {0,150,0},
        130,
        (material) {
            (vect3) {50,0,100},
            (vect3) {1,1,1},
            0.4
        }
    };
    spheres[4] = (sphere) {
        (vect3) {10,10,15},
        7.5,
        (material) {
            (vect3) {0,50,50},
            (vect3) {1,1,1},
            0.1
        }
    };

    vect3_i pixmap[WIDTH][HEIGHT];
    vect3 origin = (vect3) {0,0,0}; 

    unsigned int sphere_count = sizeof(spheres)/sizeof(sphere);

    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            double x = j - (HEIGHT/2);
            double y = i - (WIDTH/2);

            vect3 direction = (vect3) {x,y,FOCAL_LENGHT};
            direction = norm(direction);

            vect3 col = (vect3) {0,0,0};
            vect3 result;
            for (int sample = 0; sample < SAMPLE_RATE; ++sample) {
                result = draw(origin, direction, spheres, sphere_count, MAX_STEPS);
                col = add(result, col);
            }
            col = mult(col, 1/SAMPLE_RATE);
            pixmap[i][j] = (vect3_i) {round(col.x), round(col.y), round(col.z)}; 
        }
    }
    ppm_save(pixmap, "out.ppm");

}
