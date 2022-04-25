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

    cl_int err;
    cl_device_type id;
    std::vector < cl::Device > devices;
    std::vector < cl::Platform > platforms;
    std::vector < cl_context_properties > contextProperties;
    // Get OpenCL platforms
    err = cl::Platform::get(&platforms);
    if (err != CL_SUCCESS) {
        std::cerr << "CL Platform: " << err << std::endl;
    }

    // Acquire context
    context = cl::Context(CL_DEVICE_TYPE_ALL, NULL, NULL, NULL, &err);
    if (err != CL_SUCCESS) {
        std::cerr << "CL Context: " << err << std::endl;
    }
    // Get context properties
    contextProperties = context.getInfo<CL_CONTEXT_PROPERTIES>();
    if (contextProperties.size() == 0) {
        std::cerr << "CL Properties: " << err << std::endl;
    }
    // Get context devices
    devices = context.getInfo<CL_CONTEXT_DEVICES>();
    if (devices.size() == 0) {
        std::cerr << "CL Devices: " << err << std::endl;
    }
    // Select the GPU
    for (int i = 0; i < devices.size(); i++) {
        if (devices[i].getInfo<CL_DEVICE_TYPE>() == CL_DEVICE_TYPE_GPU)
            id = i;
    }

    std::cout << "Device : "       << devices[id].getInfo<CL_DEVICE_NAME>() << std::endl;
    std::cout << "Vendor : "       << devices[id].getInfo<CL_DEVICE_VENDOR>() << std::endl;
    std::cout << "CU     : "       << devices[id].getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>() << std::endl;
    std::cout << "Work Item   : "  << devices[id].getInfo<CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS>() << std::endl;
    std::cout << "Worg GrSize : "  << devices[id].getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>() << std::endl;
    std::cout << "Clock    : "  << devices[id].getInfo<CL_DEVICE_MAX_CLOCK_FREQUENCY>() << std::endl;
    std::cout << "Mem      : "  << devices[id].getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>() / (1024 * 1024) << std::endl;
    std::cout << "Cache    : "  << devices[id].getInfo<CL_DEVICE_GLOBAL_MEM_CACHE_SIZE>() / (1024) << " Kb" << std::endl;
    std::cout << "Variable : "  << devices[id].getInfo<CL_DEVICE_MAX_GLOBAL_VARIABLE_SIZE>() / (1204 * 1024) << std::endl;

    // Prepare a command queue
    queue = cl::CommandQueue(context, devices[id], 0, &err);
    if (err != CL_SUCCESS) {
        std::cerr << "CL CommandQueue: " << err << std::endl;
    }

    iCells = cl::Buffer(context, CL_MEM_READ_ONLY, rows * cols * sizeof(uint8_t));
    oCells = cl::Buffer(context, CL_MEM_WRITE_ONLY, rows * cols * sizeof(uint8_t));
    createKernel(devices);

    cells = new uint8_t[rows * cols];
    area->set_cells(cells);
    add(*area);
    show_all();
    init();

    Glib::signal_timeout().connect( sigc::mem_fun(*this, &GolWin::advance), delay);

    std::cout << "GoLmm: world " << cols << " x " << rows << " cells, cycle time " << delay << " msec" << std::endl;
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
    //cells[cols * 2 + 2] = 1;
    //cells[cols * 3 + 2] = 1;
    //cells[cols * 4 + 2] = 1;

    std::cout << std::endl;
    queue_draw();
}

gboolean GolWin::advance(void)
{
    int size =  rows * cols;
    uint8_t *next = new uint8_t[rows * cols];

    auto start = std::chrono::high_resolution_clock::now();

    // set input buffer
    queue.enqueueWriteBuffer(iCells, CL_TRUE, 0, rows * cols * sizeof(uint8_t), cells, NULL, &event);
    event.wait();
    // set parameters
    kernel.setArg(0, iCells);
    kernel.setArg(1, rows);
    kernel.setArg(2, cols);
    kernel.setArg(3, oCells);
    // run kernel
    queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(rows * cols), cl::NullRange, NULL, &event);
    event.wait();
    // read output buffer
    queue.enqueueReadBuffer(oCells, CL_TRUE, 0, rows * cols * sizeof(uint8_t), cells, NULL, &event);
    event.wait();

    auto mid = std::chrono::high_resolution_clock::now();

//    bzero(next, sizeof(uint8_t) * size);
//    for (int i = 0; i < size; ++i) {
//        int x, y;
//        int j, v, h;
//        int c;
//
//        c = 0;
//        for (v = -1; v <= 1; ++v) {
//            y = (i / cols) + v;
//            if (y < 0)
//                y += rows;
//            y %= rows;
//            for (h = -1; h <= 1; ++h) {
//                x = (i % cols) + h;
//                if (x < 0)
//                    x += cols;
//                x %= cols;
//
//                j = x + y * cols;
//                if ((i != j) && (cells[j] != 0)) {
//                    ++c;
//                }
//            }
//        }
//
//        if (cells[i]) {
//            if ((c == 2) || (c == 3)) {
//                next[i] = 1;
//            } else {
//                next[i] = 0;
//            }
//        } else {
//            if ((c == 3) || (c == 6)) {
//                next[i] = 1;
//            }
//        }
//    }
//    memcpy(cells, next, size * sizeof(uint8_t));

    queue_draw();

    auto end = std::chrono::high_resolution_clock::now();

    int time1 = std::chrono::duration_cast<std::chrono::microseconds>(mid - start).count();
    int time2 = std::chrono::duration_cast<std::chrono::microseconds>(end - mid).count();

    std::cout << "cycle time [usec]: " << time1 << " + " << time2 << std::endl;
    if ((time1 + time2) / 1000  > delay) {
        std::cerr << "delay time is too short, increase at least to " << (time1 + time2) / 1000 << std::endl;
        //close();
        //return false;
    }

    return true;
}

void GolWin::createKernel(const std::vector<cl::Device> &devices)
{
    cl_int err;
    cl::Program::Sources obj;
    cl::Program program;
    std::string kernelSource(
#include "kernel.cl"
            );

    program = cl::Program(context, kernelSource, false, &err);
    if (err != CL_SUCCESS) {
        std::cerr << "CL Programm: " << err << std::endl;
    }
    program.build(devices);
    kernel = cl::Kernel(program, "process", &err);
    if (err != CL_SUCCESS) {
        std::cerr << "CL Kernel: " << err << std::endl;
    }

}
