#!/usr/bin/ruby

require 'gtk2'

class QuadTriApp < Gtk::Window
   def initialize
      super
      set_title "QuadTri - Pawel Pikula,Michal Niec"
      signal_connect "destroy" do 
          Gtk.main_quit
      end

      init_menubar

      set_default_size 800,600
      set_window_position Gtk::Window::POS_CENTER

      show
   end

   def init_menubar
     
     vbox = Gtk::VBox.new false, 2
     menu_bar = Gtk::MenuBar.new 


     file_item = Gtk::MenuItem.new "File"
     file_menu = Gtk::Menu.new
     help_item =Gtk::MenuItem.new "Help" 
     help_menu = Gtk::Menu.new 
     
     open_item = Gtk::MenuItem.new "Open" 
     close_item = Gtk::MenuItem.new "Close"

     # menu item handlers 
     # default close operation
     close_item.signal_connect("activate") { Gtk.main_quit }
     open_item.signal_connect("activate") { on_open_clicked }

     # show filemenu items 
     open_item.show
     close_item.show

     file_menu.append open_item 
     file_menu.append close_item 
     
     file_item.set_submenu file_menu
     file_item.show 
 
     about_item = Gtk::MenuItem.new "About" 
     about_item.signal_connect("activate") { on_about_clicked } 
     about_item.show 

     help_menu.append about_item 
     
     help_item.set_submenu help_menu
     help_item.show
    
     # attach menus to menubar
     menu_bar.append file_item
     menu_bar.append help_item

     #add menu_bar 
     menu_bar.show 

     #add vbox
     vbox.pack_start menu_bar, false, false, 0
     vbox.show
     add vbox
   end

   def on_open_clicked
     dialog=Gtk::FileChooserDialog.new("Open File",self, Gtk::FileChooser::ACTION_OPEN,nil,
                                       [Gtk::Stock::CANCEL, Gtk::Dialog::RESPONSE_CANCEL],
				       [Gtk::Stock::OPEN, Gtk::Dialog::RESPONSE_ACCEPT])

     if dialog.run == Gtk::Dialog::RESPONSE_ACCEPT
     	puts dialog.filename
	#TODO read file here - it should be compatible with geomviewer poly  
     end 

     dialog.destroy
   end 

   def on_about_clicked
     dialog = Gtk::Dialog.new("About",self,Gtk::Dialog::DESTROY_WITH_PARENT,[ Gtk::Stock::OK, Gtk::Dialog::RESPONSE_NONE ])
     dialog.signal_connect('response') { dialog.destroy }
     title = Gtk::Label.new 
     title.set_markup("<big><b>Quadtree triangulation</b></big>")
     dialog.vbox.add(title)
     dialog.vbox.add(Gtk::Label.new("TODO: Description"))
     dialog.vbox.add(Gtk::Label.new("Michal Niec (michalniec@gmail.com)\nPawel Pikula (ppikula@gmail.com)"))
     dialog.set_default_size 400,200
     dialog.show_all
   end 

   def init_canvas 
   #TODO init canvas here 
   end 


end


Gtk.init 
    window=QuadTriApp.new 

Gtk.main

