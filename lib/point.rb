class Point
  attr_accessor :x,:y,:props
  
  def self.from_s(string)
    _,x,y = *string.match(/([0-9.]+) ([0-9.]+)/)
    return Point.new(x.to_f,y.to_f)
  end
  
  def initialize(x,y,props={})
    @x=x
    @y=y
    @props = props
  end
  
  def to_s
    "#{@x} #{@y}" 
  end
  
  def to_a
    [@x,@y]
  end
  
  def <=>(other)
    res = self.x <=> other.x 
    if res == 0
      return self.y <=> other.y 
    else
      return res
    end
  end
end

class Points < Array
  
  def self.random(quant,x_bound,y_bound,to_float=true)
    arr = Points.new
    r = Random.new(Time.now.to_i)
    x_bound.sort!
    y_bound.sort!
    
    1.upto quant do |i|
      x= r.rand() *(x_bound.last-x_bound.first)+x_bound.first
      y= r.rand() *(y_bound.last-y_bound.first)+y_bound.first
      (x,y = x.round,y.round) unless to_float
      arr << Point.new(x,y)
    end
    
    return arr
  end
  
  def add_uniq(point)
    if not self.any?{|p| (p <=> point) == 0}
      self << point
    end
    self
  end
  
  def get_bounds
    x_min,x_max = self.min_x.x,self.max_x.x
    y_min,y_max = self.min_y.y,self.max_y.y
    [Point.new(x_min,y_min),Point.new(x_max,y_min),Point.new(x_max,y_max),Point.new(x_min,y_max)]
  end
  
  def sort_by_x
    self.sort{|p1,p2| p1.x<=>p2.x}
  end
  
  def sort_by_y
    self.sort{|p1,p2| p1.y<=>p2.y}
  end
  
  def min_x 
    self.min{|p1,p2| p1.x <=>p2.x}
  end
  
  def min_y 
    self.min{|p1,p2| p1.y <=>p2.y}
  end
  
  def max_x 
    self.max{|p1,p2| p1.x <=>p2.x}
  end
  
  def max_y 
    self.max{|p1,p2| p1.y <=>p2.y}
  end

  def to_a
    self.map do |p|
      p.to_a
    end
  end
  
  def to_s
    buff = "POINTS\n"
    self.each do |p|
      buff << p.to_s + "\n"
    end
    buff << "END"
    return buff
  end
  
end
