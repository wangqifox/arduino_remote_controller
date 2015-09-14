from flask import Flask

app = Flask(__name__)
app.secret_key = 'shsdfgerwtgjghfj456345s'

from app.views import arduino
