# -*- coding: utf-8 -*-
import sys
reload(sys)
sys.setdefaultencoding('utf-8')

from flask import Flask, request, session, render_template, g, jsonify, redirect, url_for
from app import app
from app.models import *
import datetime
from functools import wraps
from utils import *

needlogin = True

def need_login(f):
	@wraps(f)
	def check(*args, **kwargs):
		if needlogin and not session.get('token', None):
			 return redirect(url_for('login'))
		return f(*args, **kwargs)
	return check

@app.before_request
def before_request():
	g.db = database
	g.db.connect()
	g.token = session.get('token', None)

@app.after_request
def after_request(response):
	g.db.close()
	return response

@app.route('/login', methods = ['POST', 'GET'])
def login():
	if request.method == 'GET':
		return render_template('login.html')
	elif request.method == 'POST':
		print request
		token = request.form.get('token')
		print token
		try:
			token = Token.get(Token.token==token)
			session['token'] = token.token
			return redirect(url_for('home'))
		except:
			return render_template('login.html', err='登入失败，请检查序列号')

@app.route('/logout', methods = ['POST', 'GET'])
def logout():
	session.pop('token', None)
	return render_template('login.html') 

@app.route('/')
@need_login
def home():
	return render_template('home.html', token=g.token)

@app.route('/add_action', methods = ['POST', 'GET'])
@need_login
def add_action():
	'''
	增加动作
	'''
	r = request.args if request.method == 'GET' else request.form
	action_id = r.get('action_id')

	print action_id, g.token
	try:
		token = Token.get(Token.token==g.token)
		action = Action.get(Action.id==action_id)
		create_time = datetime.datetime.today()
		actionlist = ActionList(create_time=create_time, token=token, action=action)
		actionlist.save()
		result = setSuccessMsg('命令发送成功')
	except Token.DoesNotExist:
		result = setErrorMsg('序列号错误')
	except Action.DoesNotExist:
		result = setErrorMsg('没有这个命令')
	except Exception as e:
		print e
		result = setErrorMsg('命令发送失败')

	
	return jsonify(result)
