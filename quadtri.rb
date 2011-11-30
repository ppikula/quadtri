#!/usr/bin/ruby

require 'gtk2'
require 'cairo'
require './lib/cg.rb'
require './drawer.rb'

class QuadTriApp < Gtk::Window
  attr_accessor :area,:vbox
  
  def initialize
    super
    set_title "QuadTri - Pawel Pikula,Michal Niec"
    
    signal_connect "destroy" do 
      Gtk.main_quit
    end
    
    init_components
    
    set_default_size 1000,900
    set_window_position Gtk::Window::POS_CENTER
    
    show_all
    
    poly = Polygon.random(1000,[0,1000],[0,2000])
    
    
    1.upto 10 do |i|
      @area.add_to_queue(0.4) do |ct|
        ct.set_source_rgba 0, 0, 1, i*0.1
        ct.rectangle 50*i, 20, 40, 40
        ct.fill
      end
    end
    @area.draw
    
    
    # @area.draw_poly(poly)
  end
  
  def init_components
    @vbox = Gtk::VBox.new false, 2
    @vbox.show
    add @vbox
    @area = Drawer.new
    vbox.add(@area)
  end
  
end

if __FILE__ == $PROGRAM_NAME
  Gtk.init 
  window=QuadTriApp.new 

  Gtk.main
end


