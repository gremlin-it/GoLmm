/*
 * cgolwin.cpp
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
#include <math.h>
#include <iostream>
#include <assert.h>
#include <cmath>
#include <random>
#include <complex>
#include <chrono>

#include "golwin.h"

GolWin::GolWin(int w, int h, int s, int d): width(w), height(h), cell_size(s), delay(d)
{
    rows = height / cell_size;
    cols = width / cell_size;

    area = std::make_unique<GolArea>(rows, cols);

    set_show_menubar(false);
    set_default_size(width, height);
    set_title("gremlin's Life");
    set_decorated();
    set_resizable(false);

    cells = new uint8_t[rows * cols];
    area->set_cells(cells);
    add(*area);
    show_all();
    init();

    Glib::signal_timeout().connect( sigc::mem_fun(*this, &GolWin::advance), 30);
}

GolWin::~GolWin()
{
    delete[] cells;
}

void GolWin::init(void)
{
    int size =  rows * cols;
    std::random_device rdev;
    std::mt19937 rgen(rdev());
    std::uniform_int_distribution<int> d(0, 9);

    bzero(cells, sizeof(uint8_t) * size);
    for (int i = 0; i < size; ++i) {
        cells[i] = (d(rgen)==0) ? 1 : 0;
    }
    //cells[cols * 1 + 1] = 1;
    //cells[cols * 2 + 1] = 1;
    //cells[cols * 3 + 1] = 1;

    std::cout << std::endl;
    queue_draw();
}

gboolean GolWin::advance(void)
{
    int size =  rows * cols;
    uint8_t *next = new uint8_t[rows * cols];

    auto start = std::chrono::high_resolution_clock::now();
    bzero(next, sizeof(uint8_t) * size);
    for (int i = 0; i < size; ++i) {
        int x, y;
        int j, v, h;
        int c;

        c = 0;
        for (v = -1; v <= 1; ++v) {
            y = (i / cols) + v;
            if (y < 0)
                y += rows;
            y %= rows;
            for (h = -1; h <= 1; ++h) {
                x = (i % cols) + h;
                if (x < 0)
                    x += cols;
                x %= cols;

                j = x + y * cols;
                if ((i != j) && (cells[j] != 0)) {
                    ++c;
                }
            }
        }

        if (cells[i]) {
            if ((c == 2) || (c == 3)) {
                next[i] = 1;
            } else {
                next[i] = 0;
            }
        } else {
            if ((c == 3) || (c == 6)) {
                next[i] = 1;
            }
        }
    }
    memcpy(cells, next, size * sizeof(uint8_t));
    auto end = std::chrono::high_resolution_clock::now();
    int cycle = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    if (cycle > delay) {
        std::cerr << "delay time is too short, increase at leat to " << cycle << std::endl;
        close();
        return false;
    }

    queue_draw();

    return true;
}
