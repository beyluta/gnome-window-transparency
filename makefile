all:
	gcc -o gnome-window-transparency src/main.c src/window.c -lxcb -Iinclude