from Tkinter import *
import tkMessageBox
from FreesteelPython import *
import code

class MainFrame(Frame):
	
	def selectNone(self):
		self.vtkwindow.display()
	def selectCone(self):
		self.vtkwindow.display(self.conedata)
	def selectSphere(self):
		self.vtkwindow.display(self.spheredata)
	def showConsole(self):
		cons = code.InteractiveConsole(locals())
		cons.interact()
	def onQuit(self):
#		if tkMessageBox.askokcancel("Quit", "Do you really wish to quit?"):
		self.root.destroy()

	def onSizeVtk(self, event):
		self.vtkwindow.setSize(event.width, event.height)

	def onKey(self, event):
		print event.char

	def __init__(self, master=None):
		self.root = Tk()

		Frame.__init__(self, master)
		self.master.title("Freesteel")
		self.pack(fill=BOTH, expand=1)
		self.root.protocol("WM_DELETE_WINDOW", self.onQuit)

		# Menus

		self.menubar = Menu(self.root)
		self.root.config(menu=self.menubar)

		self.filemenu = Menu(self.menubar, tearoff=0)
		self.filemenu.add_command(label="Quit!", command=self.onQuit)
		self.menubar.add_cascade(label="File", menu=self.filemenu)

		self.objectmenu = Menu(self.menubar, tearoff=0)
		self.objectmenu.add_command(label="None", command=self.selectNone)
		self.objectmenu.add_command(label="Cone", command=self.selectCone)
		self.objectmenu.add_command(label="Sphere", command=self.selectSphere)
		self.menubar.add_cascade(label="Objects", menu=self.objectmenu)

		self.toolsmenu = Menu(self.menubar, tearoff=0)
		self.toolsmenu.add_command(label="Console", command=self.showConsole)
		self.menubar.add_cascade(label="Tools", menu=self.toolsmenu)

		# Frame around VTK window

		self.borderfrm = Frame(master=self, bd=1, relief=SUNKEN)
		self.borderfrm.pack(fill=BOTH, expand=1)

		# The actual VTK window

		# create a TK window to use, bg="" means no paint-handler, so the window can be used by the VTK client
		self.vtk = Frame(master=self.borderfrm, width=400, height=400, bg="", colormap="new", relief=SUNKEN)
		self.vtk.pack(fill=BOTH, expand=1)
		self.vtk.bind("<Configure>", self.onSizeVtk)
		self.vtk.bind("<Key>", self.onKey)

		# retrieve window id (e.g. HWND on Win32)
		self.vtkfrmwinid = int(self.vtk.tk.call("winfo", "id", self.vtk._w), 16)

		# create the VtkWindow-derived window - in the TK frame...
		self.vtkwindow = PolydataWindow(self.vtkfrmwinid)

		# Initialize data objects

		self.conedata = VtkCone()
		self.spheredata = VtkSphere()

		# Initial display

		self.vtkwindow.display(self.spheredata)

# Test
#pdw = PolydataWindow()
#pdw.start() # enter main loop
mainframe = MainFrame()
mainframe.mainloop()
