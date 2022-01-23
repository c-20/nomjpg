#if 0
gcc=g++
src=nomjpg.cc
out=nomjpg
# includes="-I./CImg"
includes=""
warnings="-Wall -Wextra -Wfatal-errors -Werror=unknown-pragmas -Werror=unused-label -Wshadow"
# standard="-std=c++11 -pedantic" # filesystem requires C++17
standard="-std=c++17 -pedantic"
defines="-Dcimg_use_vt100 -Dcimg_display=1"
libs="-lm -lX11 -lpthread"
$gcc -o $out $src $includes $warnings $standard $defines $libs
exit 0
#else
#include "CImg.h"

#include <string>
#include <vector>
#include <filesystem>
#include <iostream>
#include <sstream>


using namespace cimg_library;


#ifndef cimg_imagepath
#define cimg_imagepath "img/"
#endif

#define nomjpg  main
#define IN      int
#define CH      char
#define CCH     const CH
#define CCS     CCH *
#define $$$     IN argc, CH **argv

#define LOG                  std::cout
#define STRING               std::string
#define VECTOR               std::vector
#define FSPATH               VECTOR<std::filesystem::path>
#define DIRITER(folderstr)   std::filesystem::directory_iterator(folderstr)
#define NPOS                 std::string::npos
//#define CIMAGE(pathstr)      CImg<unsigned char>(pathstr)
//#define CCIMAGE              const CImg<unsigned char>
#define CIMAGE               const CImg<unsigned char>
#define CWINDOWWIDTH         1024
#define CWINDOWHEIGHT        768

STRING IN2STR(IN x) { std::stringstream ss; ss << x; return ss.str(); }

//IN main($$$) {
IN nomjpg($$$) {
  IN cwinwidth = CWINDOWWIDTH;
  IN cwinheight = CWINDOWHEIGHT;
  cimg_usage("Trim an image with wasd/jk, confirm with c (crop/cycle)"); // -h
  STRING imgfolder = "./img";
  STRING omgfolder = "./omg";
  FSPATH imglist;
  std::copy(DIRITER(imgfolder), DIRITER(), std::back_inserter(imglist));
  std::sort(imglist.begin(), imglist.end());

//for (const std::string & filename : files_in_directory) {
//    std::cout << path.string() << std::endl; // printed in alphabetical order
//}
  CImgDisplay imagewindow(cwinwidth, cwinheight, "NO IMAGE", 0);
//  IN cropmode = 1; // crop until uncrop
  IN numfiles = (IN)imglist.size();
  IN fileix = 0; // align to first image
  IN prevfileix = -1; // initialise first image
  // defined when image is loaded (also loads outcache)
  IN imagewidth = 0, imageheight = 0;
  IN cropleft = 0, cropright = 0;
  IN cropup = 0, cropdown = 0;
  IN omgimagewidth = 0, omgimageheight = 0;
  IN updatetitle = 2; // simulate keyup
  IN titleupdates = 0;
  STRING omgerror = "";
  STRING imgpath = "DUMMYIMAGE.JPG";
  STRING omgpath = ""; // empty = no write/read
  CImg image; // starts NULL, updatetitle loads first image
  CImg croppedimage; // croppedimage ready upon display
  while (1) { // display file 0 on first iteration
    if (updatetitle == 0) {
      cimg::wait(200);  // 200ms before a second press will register
    } else if (updatetitle == 1) {
      cimg::wait(10);   // 100px/s crop
      if (++titleupdates > 99) // i got 99 updates so i
        { updatetitle = 2; }   // simulate a keyup (redraw image)
    } // else if updatetitle == 2 here then it went unhandled last iteration (or, init)
    if (fileix < 0 || fileix >= numfiles) { break; }
    else if (imagewindow.is_keyESC() || imagewindow.is_closed()) {
      return 0; // exit immediately
    } else if (imagewindow.is_keyQ()) {
      return 0; // exit immediately
//    } else if (imagewindow.is_keyA()) {
//      if (omgimagewidth > 1) { cropleft++;  updatetitle = 1; }
//      if (imagewindow.is_keySHIFTLEFT()) { // shift+nom=move
//        if (cropright > 0)   { cropright--; updatetitle = 1; }
//      }
    // grow
    } else if (imagewindow.is_keyA()) {
      if (omgimagewidth > 1) { cropleft++;  updatetitle = 1; }
    } else if (imagewindow.is_keyD()) {
      if (omgimagewidth > 1) { cropright++; updatetitle = 1; }
    } else if (imagewindow.is_keyW()) {
      if (omgimageheight > 1) { cropup++;   updatetitle = 1; }
    } else if (imagewindow.is_keyS()) {
      if (omgimageheight > 1) { cropdown++; updatetitle = 1; }
    // move
    } else if (imagewindow.is_keyF()) {
      if (omgimagewidth > 1) { cropleft++;  updatetitle = 1; }
      if (cropright > 0)     { cropright--; updatetitle = 1; }
    } else if (imagewindow.is_keyH()) {
      if (omgimagewidth > 1) { cropright++; updatetitle = 1; }
      if (cropleft > 0 )     { cropleft--;  updatetitle = 1; }
    } else if (imagewindow.is_keyT()) {
      if (omgimageheight > 1) { cropup++;   updatetitle = 1; }
      if (cropdown > 0)       { cropdown--; updatetitle = 1; }
    } else if (imagewindow.is_keyG()) {
      if (omgimageheight > 1) { cropdown++; updatetitle = 1; }
      if (cropup > 0   )      { cropup--;   updatetitle = 1; }
    // shrink
    } else if (imagewindow.is_keyJ()) {
      if (cropleft > 0 ) { cropleft--;  updatetitle = 1; }
    } else if (imagewindow.is_keyL()) {
      if (cropright > 0) { cropright--; updatetitle = 1; }
    } else if (imagewindow.is_keyI()) {
      if (cropup > 0   ) { cropup--;    updatetitle = 1; }
    } else if (imagewindow.is_keyK()) {
      if (cropdown > 0) { cropdown--;   updatetitle = 1; }
    // save
    } else if (imagewindow.is_keyC()) {
      if (omgpath.length() > 0) {
        CCS omgcpath = omgpath.c_str();
        croppedimage.save(omgcpath);
        LOG << "SAVED: " << omgpath << "\n";
      }
    } else if (imagewindow.is_keyARROWLEFT()) {
      if (fileix > 0) { fileix--; updatetitle = 2; }
      // else startoflist animation
    } else if (imagewindow.is_keyARROWRIGHT()) {
      if (fileix < numfiles - 1) { fileix++; updatetitle = 2; }
      // else endoflist animation
    // if no keys are pressed, update the image
    } else if (updatetitle == 1) { updatetitle = 2; } // key release

    if (updatetitle > 0) { // need to update on initial init too
      if (prevfileix != fileix) {
        imgpath = imglist.at(fileix);
        size_t imgfolderfound = imgpath.find_first_of(imgfolder);
        if (imgfolderfound == 0) { // path at start of string as expected
          omgpath = omgfolder + imgpath.substr(imgfolder.length());
        } else { omgpath = ""; } // don't open the empty one !
        // update paths if index changed (on keypress as well as keyup)
      } // don't update prevfileix until image changes
std::cout << "Test3... imgpath: " << imgpath << ", omgpath: " << omgpath << ", updatetitle: " << updatetitle << "\n";

      if (updatetitle > 1) { // key released (updatetitle == 2)
        titleupdates = 0; // reset periodic keyup counter
        updatetitle = 0; // 0 until next keydown
        if (prevfileix != fileix) { // load fresh image and crop data
          // load image details and display
          // loads image each crop instead of reusing !
          CCS imgcpath = imgpath.c_str();
//          CImg image = CImg<>(imgcpath); // CIMAGE image =
          image = CImg<>(imgcpath);
          // ^^ broad context: relies on init iteration correctly loading an image
          // todo: load a default error image?
          imagewidth = image.width();
          imageheight = image.height();
std::cout << "Test4... imagewidth: " << imagewidth << ", imageheight: " << imageheight << "\n";

          // load outcache crop data if present
          cropleft = cropright = cropup = cropdown = 0; // reset before load
          if (omgpath.length() > 0) { // if outcache/outfile path is set
            CCS omgcpath = omgpath.c_str();
            FILE *omgfile = fopen(omgcpath, "r"); // read current file data
            if (omgfile) { // else no current file (stay at 0s)
              IN omgchar = fgetc(omgfile);
              IN *omgnumber = &cropleft;
              if (omgchar == EOF) {
                omgerror = "omgfile unexpectedly empty\n";
                fclose(omgfile);
              } else if (omgchar != '[') {
                omgerror = "CROPPED OR NONCROP\n"; // todo: make this normal -- store crop info?
                // detect if valid JPG and display that -- consider an uncrop from metadata option
                fclose(omgfile);
              } else { // file starts with [
                omgchar = fgetc(omgfile);
                while (omgchar != EOF) {
                  if (omgchar == ',' && omgnumber != NULL) { // 
                    if      (omgnumber == NULL      ) { omgerror = "Comma outside []\n";    break; }
                    else if (omgnumber == &cropleft ) { omgnumber = &cropright;                    }
                    else if (omgnumber == &cropright) { omgnumber = &cropup;                       }
                    else if (omgnumber == &cropup   ) { omgnumber = &cropdown;                     }
                    else if (omgnumber == &cropdown ) { omgerror = "Too many input values"; break; }
                    else                              { omgerror = "Unexpected omgnumber";  break; }
                  } else if (omgchar == ']') {
                    if      (omgnumber == NULL      ) { LOG << "  Extra ]\n";                      break; }
                    else if (omgnumber != &cropdown ) { LOG << "  Not enough crop input values\n"; break; }
                    omgnumber = NULL; // ready for exit
                  } else if (omgchar == '\n') {
                    if (omgnumber == NULL) { break; } // graceful exit
                    else { LOG << "  Linebreak before crop input list ]\n"; break; }
                  } else if (omgchar == ' ') {
                    // ignore all spaces for now (spaces within numbers should be disallowed)
                  } else if (omgchar < '0' || omgchar > '9') {
                    LOG << "  Invalid crop char.\n";
                  } else { // don't expect omgnumber to be NULL
                    *omgnumber = (*omgnumber * 10) + (omgchar - '0');
                  }
                  omgchar = fgetc(omgfile);
                }
                // if omgchar is not EOF something went wrong
                fclose(omgfile);
//            omgtitle += " [" + IN2STR(cropleft);
//            omgtitle += ", " + IN2STR(cropright);
//            omgtitle += ", " + IN2STR(cropup);
//            omgtitle += ", " + IN2STR(cropdown);
//            omgtitle += "] -> " + omgfolder;
              } // end of reading valid file that started with [
            } // else no omgfile to load details from
          } // crop details have been loaded. update the image
          prevfileix = fileix; // update index
        } // else current image didn't change
        // same image or different, any keyup we redraw the image
        omgimagewidth = imagewidth - cropleft - cropright;
        omgimageheight = imageheight - cropup - cropdown;
        IN omgrx = imagewidth - 1 - cropright;
        IN omgby = imageheight - 1 - cropdown;
        croppedimage = image.get_crop(cropleft, cropup, 0, 0, omgrx, omgby, 0, 0);
        CIMAGE sizedimage = croppedimage.normalize(0, 255).resize(cwinwidth, cwinheight);
        imagewindow.display(sizedimage);
        // after redrawing, update the outcache file
        if (omgpath.length() > 0) { // assume overwriteable (including outimage->outcache)
          CCS omgcpath = omgpath.c_str();
          FILE *omgfile = fopen(omgcpath, "w");
          if (!omgfile) {
            omgerror = "ERROR SAVING OUTCACHE FILE";
            // consider file might be busy, opened by another program!
          } else { // write crop numbers. todo: write source image name ?
            fprintf(omgfile, "[%d, %d, %d, %d]\n", cropleft, cropright, cropup, cropdown);
            fclose(omgfile);
          }
        } // else there is no omgfile to write to for some reason
        // not every keypress for the sake of the hard drive ...
        // after loading, title will be outdated ......... update title BELOW !!!!
      } else { // that was keyup, this is keypress (writes a file once per keypress!!
        // and re-crop and re-display the current image
        // init will have nonmatching prevfileindex, processed above
        omgimagewidth = imagewidth - cropleft - cropright;
        omgimageheight = imageheight - cropup - cropdown;
        IN omgrx = imagewidth - 1 - cropright;
        IN omgby = imageheight - 1 - cropdown;
        croppedimage = image.get_crop(cropleft, cropup, 0, 0, omgrx, omgby, 0, 0);
        CIMAGE sizedimage = croppedimage.normalize(0, 255).resize(cwinwidth, cwinheight);
        imagewindow.display(sizedimage);
      } // end of switch image vs update current cache condition (keyup else keypress)
      // title updates with each keypress iteration
      STRING omgtitle = "IX: " + IN2STR(fileix);
      omgtitle += ": " + imgpath;
      if (updatetitle == 1 && prevfileix != fileix) {
        omgtitle += "[]"; // if waiting for an image change, current crop data is meaningless ....
        // wait for keyup (if updatetitle == 2, image index aligned above)
        // can shift arrow key more than once --- title should flick through but don't load huge images
      } else {
        omgtitle += " [" + IN2STR(cropleft);
        omgtitle += ", " + IN2STR(cropright);
        omgtitle += ", " + IN2STR(cropup);
        omgtitle += ", " + IN2STR(cropdown);
        omgtitle += "] -> " + omgfolder;
        omgtitle += " [" + IN2STR(omgimagewidth);
        omgtitle += "x" + IN2STR(omgimageheight);
        omgtitle += "]";
      }
      if (omgerror.length() > 0) {
        omgtitle += "!!! " + omgerror;
      }
      imagewindow.set_title("((( %s )))", omgtitle.c_str());
    } // end of updatetitle == 1 (keypress procedure)
  }
  // here is reached if we end up out of index bounds
  return 1;
}

/*

//  for (const std::string & filename : imglist) {
//    std::cout << path.string() << std::endl; // printed in alphabetical order
//      std::cout << filename << std::endl; // printed in alphabetical order
//  }
//  if (1) { return 0; }
  // Read image filename from the command line (or set it to "img/parrot.ppm" if option '-i' is not provided).
//  CCS file_i = cimg_option("-i",cimg_imagepath "parrot.ppm","Input image");
  // Read pre-blurring variance from the command line (or set it to 1.0 if option '-blur' is not provided).
//  const double sigma = cimg_option("-blur",1.0,"Variance of gaussian pre-blurring");
  // Load an image, transform it to a color image (if necessary) and blur it with the standard deviation sigma.

  // Create two display window, one for the image, the other for the color profile.
  CImgDisplay
    main_disp(image,"Color image (Try to move mouse pointer over)",0),
    draw_disp(500,400,"Color profile of the X-axis",0);

  // Define colors used to plot the profile, and a hatch to draw the vertical line
  unsigned int hatch = 0xF0F0F0F0;
  const unsigned char
    red[]   = { 255,0,0 },
    green[] = { 0,255,0 },
    blue [] = { 0,0,255 },
    black[] = { 0,0,0 };

    // Enter event loop. This loop ends when one of the two display window is closed or
    // when the keys 'ESC' or 'Q' are pressed.
    while (!main_disp.is_closed() && !draw_disp.is_closed() &&
           !main_disp.is_keyESC() && !draw_disp.is_keyESC() && !main_disp.is_keyQ() && !draw_disp.is_keyQ()) {

      // Handle display window resizing (if any)
      if (main_disp.is_resized()) main_disp.resize().display(image);
      draw_disp.resize();

      if (main_disp.mouse_x()>=0 && main_disp.mouse_y()>=0) { // Mouse pointer is over the image

        const int
          xm = main_disp.mouse_x(),                     // X-coordinate of the mouse pointer over the image
          ym = main_disp.mouse_y(),                     // Y-coordinate of the mouse pointer over the image
          xl = xm*draw_disp.width()/main_disp.width(),  // Corresponding X-coordinate of the hatched line
          x = xm*image.width()/main_disp.width(),     // Corresponding X-coordinate of the pointed pixel in the image
          y = ym*image.height()/main_disp.height();   // Corresponding Y-coordinate of the pointex pixel in the image

        // Retrieve color component values at pixel (x,y)
        const unsigned int
          val_red   = image(x,y,0),
          val_green = image(x,y,1),
          val_blue  = image(x,y,2);

        // Create and display the image of the intensity profile
        CImg<unsigned char>(draw_disp.width(),draw_disp.height(),1,3,255).
          draw_grid(-50*100.0f/image.width(),-50*100.0f/256,0,0,false,true,black,0.2f,0xCCCCCCCC,0xCCCCCCCC).
          draw_axes(0,image.width() - 1.0f,255.0f,0.0f,black).
          draw_graph(image.get_shared_row(y,0,0),red,1,1,0,255,1).
          draw_graph(image.get_shared_row(y,0,1),green,1,1,0,255,1).
          draw_graph(image.get_shared_row(y,0,2),blue,1,1,0,255,1).
          draw_text(30,5,"Pixel (%d,%d)={%d %d %d}",black,0,1,16,
                    main_disp.mouse_x(),main_disp.mouse_y(),val_red,val_green,val_blue).
          draw_line(xl,0,xl,draw_disp.height() - 1,black,0.5f,hatch=cimg::rol(hatch)).
          display(draw_disp);
      } else
        // else display a text in the profile display window.
        CImg<unsigned char>(draw_disp.width(),draw_disp.height()).fill(255).
          draw_text(draw_disp.width()/2 - 130,draw_disp.height()/2 - 5,"Mouse pointer is outside the image",
                    black,0,1,16).display(draw_disp);

      // Temporize event loop
      cimg::wait(20);
    }

    return 0;
}
*/
#endif
