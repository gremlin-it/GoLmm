/*
 * golarea.cpp
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

#include "golarea.h"
#include <cmath>
#include <gtkmm.h>
#include <gdkmm.h>
#include <cairomm/cairomm.h>
#include <cairomm/surface.h>
#include <thread>
#include <vector>
#include <chrono>

#include <iostream>
#include <cstdlib>

GolArea::GolArea(int r, int c): rows(r), cols(c)
{
    sf.resize(std::thread::hardware_concurrency());
    width = height = 0;
    std::cerr << "GolArea: " << cols << " x " << rows << std::endl;
}

GolArea::~GolArea()
{
}

void GolArea::set_cells(const uint8_t newcells[]) {
    this->cells = newcells;
};


void udraw(const Cairo::RefPtr<Cairo::Surface> &sur, const uint8_t *cells, const int start, const int end,
        const int col, const float s, int w)
{
    auto cr = Cairo::Context::create(sur);

    cr->set_line_width(1);

    cr->set_source_rgb(0.4, 0.4, 0.8);
    for (int i = start; i <= end; ++i) {
        if (cells[i] != 0) {
            int x = (i - start) % col;
            int y = (i - start) / col;
            //cr->arc(s * x + s / 2, s * y + s / 2, s / 2, 0.0, 2 * M_PI);
            cr->rectangle(s * x, s * y, s, s);
            cr->fill();
        }
    }
    cr->stroke();
    cr->set_source_rgb(0.1, 0.1, 0.1);
    for (int i = start; i <= end; ++i) {
        if (cells[i] == 0) {
            int x = (i - start) % col;
            int y = (i - start) / col;
            cr->rectangle(s * x, s * y, s, s);
            cr->fill();
        }
    }
    cr->stroke();
}

void GolArea::on_size_allocate(Gtk::Allocation& allocation)
{
    Gtk::DrawingArea::on_size_allocate(allocation);

    width = allocation.get_width();
    height = allocation.get_height();
    size = fmin(width / cols, height / rows);

    for (workers = std::thread::hardware_concurrency(); (rows %  workers) != 0; --workers);

    for (int i = 0; i < workers; ++i) {
        auto surface = Cairo::ImageSurface::create(Cairo::Format::FORMAT_RGB24, width, size * rows / workers);
        sf.at(i) = surface;
    }

    std::cerr << "GolArea::on_size_allocate workers=" << workers << std::endl;
}

bool GolArea::on_draw(const Cairo::RefPtr<Cairo::Context> &cr)
{
    //auto t0 = std::chrono::high_resolution_clock::now();
    //std::chrono::high_resolution_clock t1;

    if (cells != nullptr) {
        cr->set_line_width(1);
        cr->set_source_rgb(0.1, 0.5, 0.1);

        std::thread t[workers];
        for (int i=0; i < workers; ++i) {
            t[i] = std::thread(udraw, sf[i], cells, i * cols * (rows / workers), (i + 1) * cols * (rows / workers) - 1,
                    cols, size, i);
        }
        //std::cout << std::chrono::duration_cast<std::chrono::microseconds>(t1.now() - t0).count() << " ";
        for (int i=0; i < workers; ++i) {
            t[i].join();
            cr->set_source(sf[i], 0.0, i * (size * rows / workers));
            cr->paint();
        }
        //std::cout << std::chrono::duration_cast<std::chrono::microseconds>(t1.now() - t0).count() << " [us]"
        //        << std::endl;
    }
    return true;
}
