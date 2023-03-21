#include <cmath>
#include <vector>
#include "SobelFilter.h"

using namespace std;

SobelFilter::SobelFilter(sc_module_name n) : sc_module(n) {
  SC_THREAD(do_filter);
  sensitive << i_clk.pos();
  dont_initialize();
  reset_signal_is(i_rst, false);
}

// sobel mask
const int mask[MASK_X][MASK_Y] = {{1, 1, 1}, {1, 0, 1}, {1, 1, 1}};

void SobelFilter::do_filter() {
  int read = 0;
  int rbuffer[2][3] = {0},gbuffer[2][3] = {0},bbuffer[2][3] = {0};	
  while (true)
  {
    std::vector<int> reds, greens, blues;
    int red[3][3] = {0},green[3][3] = {0},blue[3][3] = {0};

    val_r = 0;
    val_g = 0;
    val_b = 0;

    if(!i_buffer.read()){
      for (unsigned int v = 0; v < MASK_Y; ++v){
        for (unsigned int u = 0; u < MASK_X; ++u){
          red[u][v] = i_r.read();
          green[u][v] = i_g.read();
          blue[u][v] = i_b.read();
          read++;
        }
      }
    }
    else{
      for (unsigned int v = 0; v < MASK_Y; ++v){
        for (unsigned int u = 0; u < MASK_X; ++u){
          if(u!=2){
          red[u][v] = rbuffer[u][v];
          green[u][v] = gbuffer[u][v];
          blue[u][v] = bbuffer[u][v];
          }
          else{
          red[u][v] = i_r.read();
          green[u][v] = i_g.read();
          blue[u][v] = i_b.read();
          read++;
          }
        }
      }
    }

    for (unsigned int v = 0; v < MASK_Y; ++v){
      for (unsigned int u = 0; u < MASK_X; ++u){
        val_r += red[u][v] * mask[u][v];
        reds.push_back(red[u][v]);
        val_g += green[u][v] * mask[u][v];
        greens.push_back(green[u][v]);
        val_b += blue[u][v] * mask[u][v];
        blues.push_back(blue[u][v]);
      }
    }
    std::sort(reds.begin(), reds.begin() + MASK_Y * MASK_X);
    std::sort(greens.begin(), greens.begin() + MASK_Y * MASK_X);
    std::sort(blues.begin(), blues.begin() + MASK_Y * MASK_X);

    o_result_r.write((val_r + 2*reds[reds.size() / 2])/10);
    o_result_g.write((val_g + 2*greens[greens.size() / 2])/10);
    o_result_b.write((val_b + 2*blues[blues.size() / 2])/10);

    for (unsigned int v = 0; v < MASK_Y; ++v){
      for (unsigned int u = 0; u < MASK_X-1; ++u){ 
      rbuffer[u][v] = red[u+1][v];
      gbuffer[u][v] = green[u+1][v]; 
      bbuffer[u][v] = blue[u+1][v];
      }
    }
    cout << "pixel transfer:" << read << endl;
    wait(10); // emulate module delay
  }
}
