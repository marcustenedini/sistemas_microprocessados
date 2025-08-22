class Node:
    def __init__(self):
        self.children = {}
        self.is_end = False

class Tree:
    def __init__(self):
        self.root = Node()

    def insert(self, word):
        node = self.root
        for char in word:
            if char not in node.children:
                node.children[char] = Node()
            node = node.children[char]
        node.is_end = True

    def count(self, word):
        node = self.root
        presses = 0

        for i, char in enumerate(word):
            # Sempre precisa pressionar a primeira letra
            if i == 0:
                presses += 1
            node = node.children[char]

            if node.is_end and (len(word)>i+1):
                presses+=1
                
            elif len(node.children) > 1 and (len(word)>i+1):
                presses += 1

        return presses

    # Método para printar a árvore
    def print_tree(self, node=None, level=0):
        if node is None:
            node = self.root  # Começa a partir da raiz
        for char, child_node in node.children.items():
            print('  ' * level + f"'{char}'", end="")
            if child_node.is_end:
                print(" (Fim de palavra)")
            else:
                print("")
            self.print_tree(child_node, level + 1)


# Testando a Trie e o método de contagem
tree = Tree()
num_test=int(input())
words=[]
for _ in range(0,num_test):
    temp=input()
    words.append(temp)

# Inserindo palavras
for word in words:
    tree.insert(word)

count=[]
for word in words:
    num=tree.count(word)
    count.append(num)
media= sum(count) / len(count)
print(f'{media:.2f}')
    

