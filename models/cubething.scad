difference() {
	cube(size = 3, center = true);
	union() {
		cube(size = [1, 1, 4], center = true);
		cube(size = [1, 4, 1], center = true);
		cube(size = [4, 1, 1], center = true);
	}
}