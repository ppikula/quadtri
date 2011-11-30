require './lib/cg' 

class QuadTreeNode 
	
	#level of node (how many subdivision from parent qadrant)
	attr_accessor :level
	#neighbours
	attr_accessor :N,:E,:S,:W
	attr_accessor :parent 
	#childs 
	attr_accessor :NW,:NE,:SW,:SE
	#bbox 
	attr_accessor :lu_point,:edge_size


	def initialize point,edge_size, parent=nil 
		@parent = parent 
		@edge_size = edge_size
		@lu_point = point 
		@level = parent.level+1 unless parent.nil?
	end 

	def subdivide 
		if isLeaf? 
			@NW = QuadTreeNode.new(@lu_point,@edge_size/2,self)  
			@NE = QuadTreeNode.new(Point.new(@lu_point.x+@edge_size/2,
											 @lu_point.y),
								    	     @edge_size/2,
											 self)
			@SW = QuadTreeNode.new(Point.new(@lu_point.x,
											 @lu_point.y+@edge_size/2), 
											 @edge_size/2, 
											 self)
			@SE = QuadTreeNode.new(Point.new(@lu_point.x+@edge_size/2, 
											 @lu_point.y+@edge_size/2), 
											 @edge_size, 
											 self)
			#TODO subdivide points set 
			#constriant: adjacent nodes depth difference must <= 1
			node_depth=depth()-1

			#@N.subdivide if @N.depth() < node_depth 
			#@S.subdivide if @S.depth() < node_depth
			#@E.subdivide if @E.depth() < node_depth
			#@W.subdivide if @W.depth() < node_depth

		end 
	end

	def depth 
		if isLeaf?
			@level
		else 
			[@NE.depth(),@NW.depth(),@SE.depth(),@SW.depth()].max
		end
	end 

	def isLeaf? 
		@NE.nil? 
	end

	def draw(context) 
		#TODO: recursively draw crossed line 
	end 

end 

class QuadTree
	attr_accessor :root_node 

    def initialize point, sq_size 
		@root_node = QuadTreeNode.new point,sq_size 
		@root_node.level = 0 
    end 

    def add_points_batch(points) 
    end 

    def add_point(point)
    end 

    def remove_point(point) 
    end 

    def draw
	end 

end
