import re
import sys

C = 11

files = []
for CASE in range(C):
    with open('ru%d.txt' % CASE) as fp:
        lines = fp.readlines()

    lines_access = lines[3:26]
    lines_rank = lines[29:52]
    lines_select = lines[55:77]
    lines_successor = lines[80:81] + lines[84:105]

    lines_access = map(lambda x: re.split(r'\s+', x.strip()), lines_access)
    lines_rank = map(lambda x: re.split(r'\s+', x.strip()), lines_rank)
    lines_select = map(lambda x: re.split(r'\s+', x.strip()), lines_select)
    lines_successor = map(lambda x: re.split(r'\s+', x.strip()), lines_successor)

    lines_access = list(lines_access)
    lines_rank = list(lines_rank)
    lines_select = list(lines_select)
    lines_successor = list(lines_successor)

    lines_select.append(('-', None, None, None, None))
    lines_successor.append(('-', None, None, None, None))

    files.append((lines_access, lines_rank, lines_select, lines_successor))

tex_pre = r"""
\begin{figure}[!htbp]
    \centering
    \begin{tikzpicture}
        \begin{axis}[
            width=\textwidth,
            height=\textwidth,
            legend pos=south east,
            xlabel={Ratio de compresión $\left[\frac{\text{bit}}{\text{bit}}\right]$},
            ylabel={Latencia [ns]},
            legend style={font=\scriptsize},
            ticklabel style = {font=\small},
            label style={font=\small}
        ]
            \addplot[
                scatter,only marks,scatter src=explicit symbolic,
                scatter/classes={
                    plain={mark=square*},
                    s9={mark=o,fill=black},
                    s18={mark=triangle*},
                    rrr={mark=x},
                    sd={mark=square},
                    hyb={mark=triangle}
                },
            ]
            table[x expr=\thisrow{size}/%s,y=latency,meta=label]{
                label   size    latency"""

tex_post = r"""
};
            \legend{
                \texttt{bit\_vector},
                \texttt{s9\_vector},
                \texttt{s18::vector},
                \texttt{rrr\_vector},
                \texttt{sd\_vector},
                \texttt{hyb\_vector}
            }
        \end{axis}
    \end{tikzpicture}
    \caption[Rendimiento de \textsc{%s} en un vector denso]{Rendimiento de la operación
    \textsc{%s} para un vector aleatorio con gaps de largo promedio $%s$, runs de largo promedio
    $%s$ con una probabilidad de $%s$.}
\end{figure}
"""

labels = [
    'plain',
    's9',
    's9',
    's9',
    's9',
    's9',
    's9',
    's9',
    's18',
    's18',
    's18',
    's18',
    's18',
    's18',
    's18',
    'rrr',
    'rrr',
    'rrr',
    'rrr',
    'rrr',
    'rrr',
    'sd',
    'hyb',
]

idx = {
    'Access': -8,
    'Rank': -5,
    'Select': -5,
    'Successor': -5,
}

lambda_ = 127
x = '10{,}000'
p = (.05, .1, .2, .3, .4, .5, .6, .7, .8, .9, .95)

for c in range(C):
    plain_size = files[c][0][0][-2].split('=')[-1]
    for method, data in [('Access', files[c][0])]:
        pre = tex_pre % (plain_size,)
        post = tex_post % (method, method, lambda_, x, p[c])

        in_ = ''
        for i, label in enumerate(labels):
            if i == 22 and method in ['Select', 'Successor']:
                continue
            size = files[c][0][i][-2].split('=')[-1]
            latency = data[i][idx[method]]
            if size.endswith('m'):
                size = '0.00%s' % size.replace('.', '').replace('m', '')
            in_ += ' ' * 16
            in_ += '\t'.join([label, size, latency])
            in_ += '\n'

        print(pre)
        print(in_)
        print(post)

for c in range(C):
    plain_size = files[c][0][0][-2].split('=')[-1]
    for method, data in [('Rank', files[c][1])]:
        pre = tex_pre % (plain_size,)
        post = tex_post % (method, method, lambda_, x, p[c])

        in_ = ''
        for i, label in enumerate(labels):
            if i == 22 and method in ['Select', 'Successor']:
                continue
            size = files[c][0][i][-2].split('=')[-1]
            latency = data[i][idx[method]]
            if size.endswith('m'):
                size = '0.00%s' % size.replace('.', '').replace('m', '')
            in_ += ' ' * 16
            in_ += '\t'.join([label, size, latency])
            in_ += '\n'

        print(pre)
        print(in_)
        print(post)

for c in range(C):
    plain_size = files[c][0][0][-2].split('=')[-1]
    for method, data in [('Select', files[c][2])]:
        pre = tex_pre % (plain_size,)
        post = tex_post % (method, method, lambda_, x, p[c])

        in_ = ''
        for i, label in enumerate(labels):
            if i == 22 and method in ['Select', 'Successor']:
                continue
            size = files[c][0][i][-2].split('=')[-1]
            latency = data[i][idx[method]]
            if size.endswith('m'):
                size = '0.00%s' % size.replace('.', '').replace('m', '')
            in_ += ' ' * 16
            in_ += '\t'.join([label, size, latency])
            in_ += '\n'

        print(pre)
        print(in_)
        print(post)

for c in range(C):
    plain_size = files[c][0][0][-2].split('=')[-1]
    for method, data in [('Successor', files[c][3])]:
        pre = tex_pre % (plain_size,)
        post = tex_post % (method, method, lambda_, x, p[c])

        in_ = ''
        for i, label in enumerate(labels):
            if i == 22 and method in ['Select', 'Successor']:
                continue
            size = files[c][0][i][-2].split('=')[-1]
            latency = data[i][idx[method]]
            if size.endswith('m'):
                size = '0.00%s' % size.replace('.', '').replace('m', '')
            in_ += ' ' * 16
            in_ += '\t'.join([label, size, latency])
            in_ += '\n'

        print(pre)
        print(in_)
        print(post)

