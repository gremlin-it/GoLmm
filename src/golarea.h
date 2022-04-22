/*
 * golarea.h
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

#ifndef GOLAREA_H_
#define GOLAREA_H_

#include <gdkmm/rectangle.h>
#include <gtkmm/drawingarea.h>
#include <cairomm/cairomm.h>

class GolArea : public Gtk::DrawingArea
{
public:
  GolArea(int r, int c);
  virtual ~GolArea();
  void set_cells(const uint8_t newcells[]);
protected:
  bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
  void on_size_allocate(Gtk::Allocation& allocation);
private:
  int workers = 1;
  int rows, cols;
  int width, height;
  float size = 0.0;
  std::vector<Cairo::RefPtr<Cairo::Surface>> sf;
  const uint8_t *cells = nullptr;
};

#endif /* GOLAREA_H_ */
