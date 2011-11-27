class TriangleItem
  attr_accessor :a,:b,:c,:ab_n,:bc_n,:cd_n

  
  
  def initialize(a,b,c,neighbors={})
    @a=a
    @b=b
    @c=c 
    
    @ab = Line.new(a,b)
    @bc = Line.new(b,c)
    @ca = Line.new(c,a)

    @ab_n = neighbors{:ab}
    @bc_n = neighbors{:bc}
    @cd_n = neighbors{:cd}
  end

  def next_to edge,point
    
  end
  
  def self.middle p1,p2
    x = (p1.x+p2.x)/2
    y = (p1.y+p2.y)/2
    new Point(x,y)
  end

  def self.angle p1,p2    
    mx = (p1.x+p2.x)/2
    my = (p1.y+p2.y)/2
  end
end


class Polygon
  attr_accessor :border_points,:edges

  
  
  def self.from_s(string)
    points = Points.new
    string.each_line do |l|
      if l =~ /POLY.*/
      elsif l =~ /END.*/
        return Polygon.new(points)
      else        
        points << Point.from_s(l)
      end
    end
  end
  
  def self.random(quant,x_bound,y_bound)
    points = Points.random(quant,x_bound,y_bound)
    self.new(points)
  end
  
  def initialize(border_points)
    @border_points = border_points
    @edges = []

    prev = @border_points.first 
    @border_points.each do |p|
      @edges << Line.new(prev,p)
      prev = p
    end
    
    @edges << Line.new(@border_points.first,@border_points.last)
  end
  
  
  def get_mono_lines
    y_min_index = @border_points.index(@border_points.min_y)
    y_max_index = @border_points.index(@border_points.max_y)
    
    if y_min_index < y_max_index
      rising = Points.new(@border_points[y_min_index..y_max_index])
      decreasing = Points.new(@border_points[y_max_index..-1] +  @border_points[0..y_min_index])
    else
      decreasing = @border_points[y_max_index..y_min_index]
      rising = Points.new(@border_points[y_min_index..-1] +  @border_points[0..y_max_index])
    end
    if rising[1].x > decreasing[1].x
      return [decreasing,rising]
    else
      return [rising,decreasing]
    end
  end
  
  def is_y_mono
    left,right  = get_mono_lines  
    rising,decreasing = nil
    if left[0].y>left[1].y
      decreasing = left
      rising = right
    else
      rising= left
      decreasing = right
    end
    
    return (rising == rising.sort_by_y and decreasing == decreasing.sort_by_y.reverse)
  end
  
  def to_s
    buff = "POLY color=#{(Random.new.rand*0xFFFFFF).to_i.to_s(16)}\n"
    @border_points.each do |p|
      buff << p.to_s+"\n"
    end
    buff << "END\n"
    buff
  end
  
  def triangulate
    left,right  = get_mono_lines
    left = left.sort_by_y
    right = right.sort_by_y
    
    $stderr.puts "#{left}"
    $stderr.puts "#{right}"
    
    
    sorted_bp = @border_points.sort_by_y.reverse
    stack = []
    edges = []
    stack += [sorted_bp.shift,sorted_bp.shift].reverse
    i=0
    while not sorted_bp.empty?
      
      v = sorted_bp.shift
      
      if left.include?(v) and left.include?(stack.first)
        same = :left
      elsif right.include?(v) and right.include?(stack.first)
        same = :right
      else
        same = :not_same
      end
      $stderr.puts "#{v} #{stack} #{same}"
      
      #on the same side
      if same == :right or same == :left
        #for each point from stack
        to_delete=[]
        stack[1..-1].each do |vs|
          
          #get this point
          #create a line
          f = [v,vs].sort{|p1,p2| p1.y <=> p2.y}
          l = Line.new(f[0],f[1])
          
          
          $stderr.puts "#{vs}  #{to_delete}"
          
          #for each point beetween s and e from stack 
          #check if is on the right side of the new line
          is_ok = true
          
          if same ==:right
            
            st = right.index(v)
            en = right.index(vs)
            if en!=nil
              tab = right[st+1..en-1]
              $stderr.puts "tab #{tab}"
              tab.each do |p|
                side = l.side p
                $stderr.puts "#{same} #{l} with #{p} = #{side}"
                if side <0
                  is_ok  = false    
                  break
                end
              end
            end
          else 
            st = left.index(v)
            en = left.index(vs)
            if en != nil
              tab = left[st+1..en-1]
              $stderr.puts "tab #{tab}"
              tab.each do |p|
                side = l.side p
                $stderr.puts "#{same} #{l} with #{p} = #{side}"
                if side >0
                  is_ok = false
                  break
                end
              end
            end
          end     
          
          # => line is ok? add it to the edges and remove vs from stack
          if is_ok
            
            to_delete << vs
            edges << l
          else
            
            $stderr.puts "#{l} not ok"
          end          
        end
        $stderr.puts "removal: #{stack} #{to_delete}"
        stack = stack-to_delete
        stack.insert(0,v)
      else
        stack.each do |p|
          l = Line.new(v,p)
          edges << l if not edges.any?{|x| (x <=> p)==0} 
        end
        stack = [v,stack.first]
      end
      
      puts "CLEAR"
      puts self.to_s
      puts edges
      puts "SAVE_IMG /home/n/Dev/nstudies/cg/lab4/results/#{i}.png"
      i = i.next
    end
    
    return edges
  end
  
  def to_a
    @border_points.to_a
  end
  
end
