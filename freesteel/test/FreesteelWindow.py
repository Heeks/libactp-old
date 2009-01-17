from Tkinter import *
import tkMessageBox
from FreesteelPython import *
import code

"""
Keys:
	Space: 							Start/Pause animation playback
	Backspace: 						Reverse playback direction
	+/-: 							Increase/decrease playback speed
	Home/End: 						Go to start/end of animation
	Page up/down:					10% step back/forward
	Left/Right: 					1% step back/forward
	Shift + Left/Right: 			0.1% step back/forward
	Control + Left/Right: 			0.01% step back/forward
	Shift + Control + Left/Right: 	0.001% step back/forward
"""

class MainFrame(Frame):
	
	def showConsole(self):
		cons = code.InteractiveConsole(locals())
		cons.interact()
	def onQuit(self):
#		if tkMessageBox.askokcancel("Quit", "Do you really wish to quit?"):
		self.root.destroy()

	def scroll(self, delta, absolute = False):
		if absolute:
			self.scrollpos = delta
		else:
			self.scrollpos += delta
		if( self.scrollpos < 0 ): self.scrollpos = 0
		if( self.scrollpos > 1 ): self.scrollpos = 1
		self.scrollbar.set(self.scrollpos, self.scrollpos)
		self.vtkwindow.setProgress(self.scrollpos-.1, self.scrollpos, True);
		
	def playPause(self):
		self.playing = ~self.playing
		if( self.playing ):
			self.timer = self.after(50, self.doStep)
	
	def reverseDir(self):
		self.reverse = ~self.reverse

	def doStep(self):
		if( self.reverse ):
			self.scroll(-self.step)
		else:
			self.scroll(self.step)
		if( self.playing ):
			self.timer = self.after(10, self.doStep)		
	
	def setStep(self, newstep):
		self.step = newstep

	def onScroll(self, command, pos, unit=0):
		delta = 0
		absolute = False
		if command == "scroll":
			if unit == "units":
				if pos == "1":
					delta += 0.01
				else:
					delta -= 0.01
			elif unit == "pages":
				if pos == "1":
					delta += 0.1
				else:
					delta -= 0.1
		elif command == "moveto":
			delta = float(pos)
			absolute = True
		self.scroll(delta, absolute)

	def onSizeVtk(self, event):
		self.vtkwindow.setSize(event.width, event.height)

	def onKey(self, event):
#		print event.char
		pass

	def __init__(self, master=None, width=600, height=600):
		self.root = Tk()
		
		self.step = .001
		self.reverse = False
		self.playing = False

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

#		self.objectmenu = Menu(self.menubar, tearoff=0)
#		self.menubar.add_cascade(label="Objects", menu=self.objectmenu)

		self.toolsmenu = Menu(self.menubar, tearoff=0)
		self.toolsmenu.add_command(label="Console", command=self.showConsole)
		self.menubar.add_cascade(label="Tools", menu=self.toolsmenu)

		# Frame around VTK window

		self.borderfrm = Frame(master=self, bd=1, relief=SUNKEN)
		self.borderfrm.pack(fill=BOTH, expand=1)

		# The actual VTK window

		# create a TK window to use, bg="" means no paint-handler, so the window can be used by the VTK client
		self.vtk = Frame(master=self.borderfrm, width=width, height=height, bg="", colormap="new", takefocus=1)
		self.vtk.pack(fill=BOTH, expand=1)
		self.vtk.bind("<Configure>", self.onSizeVtk)

		# retrieve window id (e.g. HWND on Win32)
		self.vtkfrmwinid = self.vtk.winfo_id()

		# create the VtkWindow-derived window - in the TK frame...
		self.vtkwindow = FreesteelWindow(self.vtkfrmwinid)
	
		# Bottom scroll bar - for animation

		self.scrollbar = Scrollbar(self, orient='horizontal', command=self.onScroll, takefocus=1)
		self.scrollbar.pack(side=BOTTOM, fill=X)
		self.scrollpos = 0.0

		# Key bindings

		# Scrolling / animation
		self.root.bind("<Key-Left>", lambda(na): self.scroll(-0.01))
		self.root.bind("<Key-Right>", lambda(na): self.scroll(0.01))
		self.root.bind("<Shift-Key-Left>", lambda(na): self.scroll(-0.001))
		self.root.bind("<Shift-Key-Right>", lambda(na): self.scroll(0.001))
		self.root.bind("<Control-Key-Left>", lambda(na): self.scroll(-0.0001))
		self.root.bind("<Control-Key-Right>", lambda(na): self.scroll(0.0001))
		self.root.bind("<Control-Shift-Key-Left>", lambda(na): self.scroll(-0.00001))
		self.root.bind("<Control-Shift-Key-Right>", lambda(na): self.scroll(0.00001))
		self.root.bind("<Key-Prior>", lambda(na): self.scroll(-0.1))
		self.root.bind("<Key-Next>", lambda(na): self.scroll(0.1))
		self.root.bind("<Home>", lambda(na): self.scroll(0, True))
		self.root.bind("<End>", lambda(na): self.scroll(1, True))
		self.root.bind("<space>", lambda(na): self.playPause())
		self.root.bind("<BackSpace>", lambda(na): self.reverseDir())
		self.root.bind("+", lambda(na): self.setStep(self.step + self.step**1.2))
		self.root.bind("-", lambda(na): self.setStep(self.step - self.step**1.2))

		self.root.bind("<Key>", self.onKey)

mainframe = None
fswindow = None
def inspect(surface, paths, boundary=None, tool=None):
	global mainframe, fswindow # Don't destroy us when function ends
	mainframe = MainFrame()
	fswindow = mainframe.vtkwindow

	fswindow.add(surface)
	fswindow.addPathxSeries(boundary)
	gsttp = GSTtoolpath()
	for path in paths:
		gsttp.addPath(path)
	gsttp.toolshape = tool
	gsttp.UpdateFromPax()
	fswindow.add(gsttp)
	fswindow.setAnimated(gsttp)

	fswindow.showAll()
	fswindow.render()

	mainframe.mainloop()

# Test
#mainframe = MainFrame()
#if ('interactive' not in sys.argv):
	# Running mainloop. Run interactively (-i) with argument 'interactive' to access the interactive console.
#	mainframe.mainloop()
