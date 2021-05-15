import tkinter
import tkinter.messagebox
import os

window=tkinter.Tk()
window.title("SMTP Client")
window.geometry("600x600")

label=tkinter.Label(window,text="SMTP Client",font=('Consolas',20),width=100,height=1)
label.pack()

smtp_label=tkinter.Label(window,text="SMTP host:",font=('Consolas',12))
usrn_label=tkinter.Label(window,text="Username :",font=('Consolas',12))
pswd_label=tkinter.Label(window,text="Password :",font=('Consolas',12))
rcvr_label=tkinter.Label(window,text="Receiver :",font=('Consolas',12))
subj_label=tkinter.Label(window,text="Subject  :",font=('Consolas',12))

smtpaddr=tkinter.Entry(window,show=None,width=50,font=('Consolas',12))
username=tkinter.Entry(window,show=None,width=50,font=('Consolas',12))
password=tkinter.Entry(window,show='*',width=50,font=('Consolas',12))
receiver=tkinter.Entry(window,show=None,width=50,font=('Consolas',12))
subject =tkinter.Entry(window,show=None,width=50,font=('Consolas',12))

smtp_label.place(x=10,y=60,anchor='nw')
smtpaddr.place(x=110,y=60,anchor='nw')
usrn_label.place(x=10,y=90,anchor='nw')
username.place(x=110,y=90,anchor='nw')
pswd_label.place(x=10,y=120,anchor='nw')
password.place(x=110,y=120,anchor='nw')
rcvr_label.place(x=10,y=150,anchor='nw')
receiver.place(x=110,y=150,anchor='nw')
subj_label.place(x=10,y=180,anchor='nw')
subject.place(x=110,y=180,anchor='nw')

content=tkinter.Text(window,width=82,height=10,font=('Consolas',10))
content.place(x=10,y=210,anchor='nw')
content.insert('end',"write content here")

info=tkinter.Text(window,width=82,height=10,font=('Consolas',10))
info.place(x=10,y=370,anchor='nw')
info.insert('end',"SMTP client info:\n")

def send_exec():
    data=[
        ('SMTP info',smtpaddr.get()),
        ('Username',username.get()),
        ('Password',password.get()),
        ('Receiver',receiver.get()),
        ('Subject',subject.get()),
        ('Content',content.get(0.0,tkinter.END))
    ]
    s=''
    for i in data:
        if(len(i[1])==0):
            s+='      '+i[0]+'\n'
    if(len(s)):
        tkinter.messagebox.showerror(title='error',message='Please input:\n'+s)
        return
    subject_file=open("subject","w")
    content_file=open("content","w")
    subject_file.write(subject.get())
    content_file.write(content.get(0.0,tkinter.END))
    subject_file.close()
    content_file.close()
    val=os.popen(
        'main '+
        smtpaddr.get()+' '+
        username.get()+' '+
        password.get()+' '+
        receiver.get(),
        "r"
    )
    info.insert('end',val.read())
    val.close()
    return
send=tkinter.Button(window,text="send",command=send_exec)
send.place(x=280,y=540,anchor='nw')

window.mainloop()