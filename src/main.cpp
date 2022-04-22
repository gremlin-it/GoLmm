/*
 * main.cpp
 *
 *  Created on: Mar 1, 2022
 *      Author: gremlin
 *
 *  This file is part of GoLpp.
 *
 *  GoLpp is free software: you can redistribute it and/or modify it under the
 *  terms of the GNU General Public License as published by the Free Software
 *  Foundation, either version 3 of the License, or (at your option) any later
 *  version.
 *  GoLpp is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 *  details.
 *  You should have received a copy of the GNU General Public License along
 *  with Golpp. If not, see <https://www.gnu.org/licenses/>.
 */

#include <iostream>
#include "golwin.h"

using std::cout;
using std::endl;

void usage(char *name)
{
    std::
        cout << "Usage: " << name <<
        " [-w window width] [-h window height] [-s size of a cell]\n";
}

int main(int argc, char *argv[])
{
    int width = 500;
    int height = 500;
    int rows = 100;
    int cols = 100;
    int size = 5;
    char action;
    int traverse = 0;

    while (++traverse < argc) {
        if (argv[traverse][0] == '-')
            action = argv[traverse][1];
        else
            action = argv[traverse][0];
        switch (action) {
        case 'w':
            if (++traverse > argc) {
                usage(argv[0]);
                exit(0);
            }
            width = atoi(argv[traverse]);
            break;
        case 'h':
            if (++traverse > argc) {
                usage(argv[0]);
                exit(0);
            }
            height = atoi(argv[traverse]);
            break;
        case 's':
            if (++traverse > argc) {
                usage(argv[0]);
                exit(0);
            }
            size = atoi(argv[traverse]);
            break;
        default:
            usage(argv[0]);
            exit(0);
            break;
        }
    }

    rows = height / size;
    cols = width / size;

    auto app = Gtk::Application::create("it.gremlin.test.gtkmmlife");

    GolWin mywin(width, height, rows, cols);
    return app->run(mywin);
}
