
scopes = {
    'global': {
        'vars': set()
    },

    'some_inner_scope': {
        'parent': 'global',
        'vars': set('hello')
    },


    'some_very_inner_scope': {
        'parent': 'some_inner_scope',
        'vars': [1, 2, 3, 4]
    },

}

for i in scopes["some_very_inner_scope"]["vars"]:
    print(i)
