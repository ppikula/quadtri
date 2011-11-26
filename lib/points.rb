#/usr/bin/ruby

Quant=ARGV.shift.to_i
Min=ARGV.shift.to_i
Max=ARGV.shift.to_i
NoClear= ARGV.shift.nil?

rand = Random.new(Time.now.to_i)
puts "CLEAR" if not NoClear
puts "POINTS COLOR FFFAAA"
1.upto Quant do |i|
  puts "#{rand.rand() *(Max-Min)+Min} #{rand.rand() *(Max-Min)+Min}"
end
puts "END"
