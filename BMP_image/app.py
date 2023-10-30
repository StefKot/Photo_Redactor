#!/usr/bin/env python
# -*- coding: utf-8 -*-
import tkinter
import os
import time
from PIL import Image, ImageDraw, ImageTk
import tkinter as tk
from tkinter import filedialog as fd
from tkinter import messagebox
from tkinter import ttk
from ctypes import *

# lib = CDLL("./libfunc.so")
# grayscale = lib.grayscale_file


class App:
    def open_image(self):
        filename = fd.askopenfilename()
        # filename = "/home/xdev/sync/INF/students/Бауманка/ИУ-7/sem3/BMP_image/Lenna.png"
        # canvas = tkinter.Canvas(self.root, height=400, width=700)
        # print(filename)
        img = Image.open(filename)
        img = img.resize((200, 200), Image.LANCZOS)

        img = ImageTk.PhotoImage(img)
        panel = tk.Label(self.root, image=img, text="Lenna")
        panel.grid(row=1, column=0)
        # panel.pack(side = "bottom", fill = "both", expand = "yes")
        panel.photo = img
        # panel.pack()

        # image = canvas.create_image(0, 0, anchor='nw', image=img)
        # canvas.photo = img
        # canvas.grid(row=2, column=0)
        self.selected_file = filename

    def load_result(self):
        filename = "Lenna2.bmp"
        img = Image.open(filename)
        img = img.resize((200, 200), Image.LANCZOS)

        img = ImageTk.PhotoImage(img)
        panel = tk.Label(self.root, image=img, text="Lenna")
        panel.grid(row=4, column=0)
        panel.photo = img

    def apply_selected(self):
        if self.selected_file is None:
            print("Choose img")
            return

        filter_name = self.selected_filter.get()
        if filter_name == "Grayscale":

            # f.argtypes = POINTER(c_char),
            # grayscale.argtypes = [c_char_p, c_char_p]
            # grayscale.argtypes = [POINTER(c_char), POINTER(c_char)]
            # f.restype = c_int
            # print(self.selected_file.encode())
            # grayscale(self.selected_file.encode(), b"Lenna2.bmp")
            lib = CDLL("./libfunc.so")
            grayscale = lib.grayscale_file
            grayscale(b"Lenna.bmp", b"Lenna2.bmp")


        # time.sleep(2)
        # self.load_result()


    def __init__(self):
        self.selected_file = None
        self.root = tk.Tk()
        self.root.title('Img processing')
        #self.root.iconbitmap(r"D:\!!!_PROGRAMMING\!!!_PYTHON\!!!_OLYMP\LABS_2sem\Lab3_26.03.22\robot.ico") # Замена иконки окна
        self.root.resizable(True, True) # изменение размера окна
        # b1 = tk.Button(text="Выбрать файл для кодирования сообщения", command = lambda: steganography_start(0), fg="black", bg="white")
        # b1.grid(row=5, column=0, sticky="we")
        # b2 = tk.Button(text="Расшифровать сообщение", command = steganography_end, fg="black", bg="white")
        # b2.grid(row=5, column=2, sticky="we")
        # b3 = tk.Button(text="Сохранить изображение с зашифрованным сообщением", command = lambda: steganography_start(1), fg="black", bg="white")
        # b3.grid(row=5, column=1, sticky="we")
        b4 = tk.Button(text="Открыть изображение", command = self.open_image, fg="black", bg="white")
        b4.grid(row=0, column=0, sticky="we")
        # ent = tk.Entry(self.root,fg="white", bg="gray", width = 30, font = ("Comic Sans MS", 10, "bold"))
        # ent.grid(column = 0, row = 1, columnspan = 4, sticky="we")

        self.selected_filter = tk.StringVar()
        combobox = ttk.Combobox(self.root, textvariable=self.selected_filter, state= "readonly")
        combobox['values'] = (
            'Grayscale',
            'Gauss blur',
            'Sobel',
            'Laplace',
            'Edges',
        )
        combobox.current(0)
        combobox.grid(row=3, column=0)


        apply = tk.Button(text="Apply filter", command = self.apply_selected)
        apply.grid(row=3, column=1)

        # Лэйблы
        # lbl = tk.Label(self.root, text = "Введите сообщение, которое хотите зашифровать: ", font = ("Comic Sans MS", 11, "bold"), fg="black", bg="white")
        # lbl.grid(column = 0, row=0, sticky="we", columnspan = 4)

        # lbl1 = tk.Label(self.root, text="Расшифрованное сообщение: ", font=("Comic Sans MS", 11, "bold"), fg="black", bg="white")
        # lbl1.grid(column=0, row=3, sticky="we", columnspan = 4)

        # lbl3 = tk.Label(self.root, text="", fg="black", bg="white")
        # lbl3.grid(column=0, row=2, sticky="we", columnspan = 4)

        # lbl2 = tk.Label(self.root, text="", font=("Comic Sans MS", 10, "bold"), fg="white", bg="gray")
        # lbl2.grid(column=0, row=4, sticky="we", columnspan = 4)


        # Меню
        # menu = tk.Menu(self.root)
        # menu_info = tk.Menu(menu)
        # menu_delete = tk.Menu(menu)
        # menu.add_cascade(label='Очистка полей', menu=menu_delete)
        # menu_delete.add_command(label = "Очистить строку ввода сообщения: ", command = lambda: string_delete(0))
        # menu_delete.add_command(label = "Очистить строку вывода сообщения: ", command = lambda: string_delete(1))
        # menu_info.add_command(label='Об авторе', command = lambda : info(0))
        # menu_info.add_command(label='Об программе', command = lambda : info(1))
        # menu.add_cascade(label='Информация', menu=menu_info)
        # # menu.add_command(label = "Выход", command = end)
        # self.root.config(menu=menu)

    def run(self):
        self.root.mainloop()


if __name__ == "__main__":
    app = App()
    app.run()
