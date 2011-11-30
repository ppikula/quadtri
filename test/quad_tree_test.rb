require 'rspec'
require './quad_tree'

#lame rspec tests

describe QuadTreeNode, "#depth" do 
	
	qtn=QuadTreeNode.new(Point.new(1024,1024),1024) 
	qtn.level=0 

	it "returns 0 for empty node" do
		qtn.depth.should == 0 
	end 

	it "returns 1 for subdivided root node" do 
		qtn.subdivide 
		qtn.depth.should == 1 
	end 

	it "returns 2 when one of subnode is subdivided" do 
		qtn.NW.subdivide
		qtn.depth.should == 2 
	end  
end 

describe QuadTreeNode,"#subdivide" do 
	root = QuadTreeNode.new(Point.new(1024,1024),1024) 
	root.level=0

	it "Quadrants depth should be NW(3) NE(2) SW(2) SE(1)  " do 
		root.subdivide 
		root.NE.subdivide 
		root.NE.SW.subdivide
		root.NE.SE.subdivide
		root.NE.NE.subdivide
		
		root.depth().should == 3
		root.NE.depth().should == 2 
		root.SW.depth().should == 2
		root.SE.depth().should == 1

	end
end 


describe QuadTree, "#addPoint" do 
	
end 

