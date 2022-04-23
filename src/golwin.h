/*
 * cgolwin.h
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

#include <memory>
#include <array>
#include <memory>
#include <gtkmm.h>
#include <random>

#include "golarea.h"

class GolWin: public Gtk::ApplicationWindow {
public:
    GolWin(int w, int h, int s, int d);
    ~GolWin();
protected:
    static const int Ncells = 100;
    gboolean advance(void);
    void init(void);
private:
    int rows, cols, width, height, cell_size, delay;
    std::unique_ptr<GolArea> area;
    uint8_t *cells;
};
