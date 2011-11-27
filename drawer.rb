
class Drawer< Gtk::DrawingArea
  attr_accessor :cairo_ct,:queue
  
  def initialize
    super
    @queue = []
    
  end
  
  
  def add_to_queue(interval=0.2,&block)
    queue << [interval,block]
  end
  
  def draw
    Thread.new do
      queue.each do |int,fun|
        @cairo_ct = self.window.create_cairo_context
        puts "drawing"
        sleep int
        fun.call @cairo_ct        
      end
    end
  end
  
end
