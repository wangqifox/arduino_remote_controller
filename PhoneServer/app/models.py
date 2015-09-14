# -*- coding: utf-8 -*-

from peewee import *
import ConfigParser
config = ConfigParser.ConfigParser()
config.read('default.ini')

print config.items('database')

database = MySQLDatabase(config.get('database', 'name'), 
	host=config.get('database', 'host'), 
	user=config.get('database', 'user'), 
	password=config.get('database', 'pass'), 
	port=int(config.get('database', 'port')),
)

class BaseModel(Model):
	class Meta:
		database = database

class Action(BaseModel):
	description = CharField(max_length=255)
	class Meta:
		db_table = 'action'

class Token(BaseModel):
	token = CharField(max_length=255)
	class Meta:
		db_table = 'token'

class ActionList(BaseModel):
	create_time = DateTimeField()
	token = ForeignKeyField(Token, db_column='token_id')
	action = ForeignKeyField(Action, db_column='action_id')
	class Meta:
		db_table = 'action_list'

