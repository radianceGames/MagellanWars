int mydes_setkey(const char *var1) {
	return 1;
}

void mydes_cipher(const char *var1, char *var2, int var3, int var4) {
	if (var4 < 0) { var4=var4*-1; }
	int index;
	for (index=0; index<var4; index++) {
		var2[index] = var1[index];
	}
}
