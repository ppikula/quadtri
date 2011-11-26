class Line
  @@init_label = :a
  
  attr_accessor :label,:start,:en,:a,:b,:dx,:dy,:vertical
  
  def initialize(s,e,label = nil)
    label = @@init_label.next if label.nil?
    @label = label
    start,en = [s,e].sort{|p1,p2| p1.x <=> p2.x}
    @start = start
    @en = en
    
    #line is vertical
    if @en.x-@start.x == 0
      @vertical = true
    else
      @vertical = false
      @a = (@en.y-@start.y)/(@en.x-@start.x)
      @b = @start.y-@a*@start.x
    end
    
    @dx = @en.x - @start.x
    @dy = @en.y - @start.y
  end
  
  def middle
    Point.new(@start.x + @dx/2,@start.y+@dy/2)
  end
  
  def angle_to p
    m = middle
    if @dy != 0
      ang1=Math::atan2(1,-@dx/@dy)
    else
      ang1=0
    end
    if m.y-p.y != 0
      ang2=Math::atan2(p.x-m.x,p.y-m.y)
    else
      ang2=0
    end
    [ang1,ang2]
  end
  
  def in_bounds?(point)
    if point.x >= @start.x and point.x <= @en.x
      ymin,ymax=[@start,@en].sort{|p1,p2| p1 <=> p2 }
      if point.y >= ymin.y and point.y <= ymax.y
        return true
      end
    end
    false
  end
  
  def side(point)
    ((@start.x-point.x)*(@en.y-point.y))-((@start.y-point.y)*(@en.x-point.x)) <=> 0
  end
  
  def start_or_end?(point)
    @start == point or @en == point
  end
  
  def self.find_crossing(l1,l2)
    if l1.side(l2.start) == l1.side(l2.en)
      return nil
    else
      x = (l2.b-l1.b)/(l1.a-l2.a)
      y = l1.a*x+l1.b
      
      return Point.new(x,y)
    end
  end
  
  def to_s
    buf = "LINE\n"
    buf << @start.to_s
    buf << "\n"
    buf << @en.to_s
    buf << "\n"
    buf <<  "END\n"
  end
  
end
