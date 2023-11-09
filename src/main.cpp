#include <iostream>
#include <vector>
#include <unordered_map>
#include "vec.h"
#include "object.h"
#include "utils.h"

using namespace std;

int main(int argc, char* argv [ ]) {

    if (argc != 2) {
        cerr << "Usage: ./program file.txt" << endl;
        return -1;
    }

    auto contents = read_file(argv[1]);

    image img;
    ray_tracer tracer;

    vector<obj*> objects;
    vector<light*> lightings;

    parse_content(contents, objects, img, tracer, lightings);
    int max_wh = max(img.w, img.h);

    for (int x = 0; x < img.w; x++) {
        for (int y = 0; y < img.h; y++) {
            // make ray
            float sx = (2.0f * float(x) - float(img.w)) / float(max_wh);
            float sy = (float(img.h) - 2.0f * float(y)) / float(max_wh);
            vec3 ray = tracer.forward + sx * tracer.right + sy * tracer.up;
            float n_t = -1; // nearest t
            vector<float> t;
            // a map correspond to t and obj
            unordered_map<float, obj*> obj_map;

            for (obj* object : objects) {
                float temp = object->intersection(tracer.eye, ray);
                t.push_back(temp);
                obj_map[temp] = object;
            }

            // find the nearest t
            for (auto to : t) {
                if (to > 0) {
                    if (n_t == -1) n_t = to;
                    else n_t = min(n_t, to);
                }
            }

            if (n_t > 0) {
                vec4 obj_color;

                if (lightings.empty())
                    obj_color = vec4(0, 0, 0, 1);
                else
                    obj_color = linear_to_srgb(
                        compute_color(tracer.eye, ray, n_t, obj_map[n_t], lightings, objects, img, 1));
                pixel new_pix(x, y, obj_color);
                img.pixels.push_back(new_pix);
            }
        }
    }

    generate_image(img);
}
