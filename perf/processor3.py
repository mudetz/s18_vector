import re
import sys

C = 15

files = []
for CASE in range(C):
    with open('rp%d.txt' % CASE) as fp:
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
            label style={font=\small},
            xmin=0,
            ymax=500,
            xmax=1.4
        ]
"""

tex_pre2 = r"""            \addplot[
                scatter,smooth,black,scatter src=explicit symbolic,
                scatter/classes={
                    plain={mark=square*,draw=black},
                    s9={mark=o,fill=black},
                    s18={mark=triangle*},
                    rrr={mark=x},
                    sd={mark=square},
                    hyb={mark=triangle}
                },
            ]
            table[x expr=\thisrow{size}/%s,y=latency,meta=label]{
                label   size    latency
"""

tex_post2 = r"""
};
"""

tex_post = r"""            \legend{
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
\clearpage
\newpage
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
x = '100'
p = (.01, .02, .03, .04, .05, .1, .2, .3, .4, .5, .6, .7, .8, .9, .95)

for c in range(C):
    plain_size = files[c][0][0][-2].split('=')[-1]
    for method, data in [('Access', files[c][0])]:
        pre = tex_pre
        post = tex_post % (method, method, lambda_, x, p[c])

        in_ = tex_pre2 % (plain_size,)
        last_label = None
        for i, label in enumerate(labels):
            if i == 22 and method in ['Select', 'Successor']:
                continue
            if label != last_label:
                in_ += tex_post2
                in_ += tex_pre2 % (plain_size,)
            size = files[c][0][i][-2].split('=')[-1]
            latency = data[i][idx[method]]
            if int(latency) > 500:
                continue
            if size.endswith('m'):
                size = '0.00%s' % size.replace('.', '').replace('m', '')
            if float(size) / float(plain_size) > 1.4:
                continue
            in_ += ' ' * 16
            in_ += '\t'.join([label, size, latency])
            in_ += '\n'
            last_label = label
        in_ += tex_post2

        print(pre)
        print(in_)
        print(post)

for c in range(C):
    plain_size = files[c][0][0][-2].split('=')[-1]
    for method, data in [('Rank', files[c][1])]:
        pre = tex_pre
        post = tex_post % (method, method, lambda_, x, p[c])

        in_ = tex_pre2 % (plain_size,)
        last_label = None
        for i, label in enumerate(labels):
            if i == 22 and method in ['Select', 'Successor']:
                continue
            if label != last_label:
                in_ += tex_post2
                in_ += tex_pre2 % (plain_size,)
            size = files[c][0][i][-2].split('=')[-1]
            latency = data[i][idx[method]]
            if int(latency) > 500:
                continue
            if size.endswith('m'):
                size = '0.00%s' % size.replace('.', '').replace('m', '')
            if float(size) / float(plain_size) > 1.4:
                continue
            in_ += ' ' * 16
            in_ += '\t'.join([label, size, latency])
            in_ += '\n'
            last_label = label
        in_ += tex_post2

        print(pre)
        print(in_)
        print(post)

for c in range(C):
    plain_size = files[c][0][0][-2].split('=')[-1]
    for method, data in [('Select', files[c][2])]:
        pre = tex_pre
        post = tex_post % (method, method, lambda_, x, p[c])

        in_ = tex_pre2 % (plain_size,)
        last_label = None
        for i, label in enumerate(labels):
            if i == 22 and method in ['Select', 'Successor']:
                continue
            if label != last_label:
                in_ += tex_post2
                in_ += tex_pre2 % (plain_size,)
            size = files[c][0][i][-2].split('=')[-1]
            latency = data[i][idx[method]]
            if int(latency) > 500:
                continue
            if size.endswith('m'):
                size = '0.00%s' % size.replace('.', '').replace('m', '')
            if float(size) / float(plain_size) > 1.4:
                continue
            in_ += ' ' * 16
            in_ += '\t'.join([label, size, latency])
            in_ += '\n'
            last_label = label
        in_ += tex_post2

        print(pre)
        print(in_)
        print(post)

for c in range(C):
    plain_size = files[c][0][0][-2].split('=')[-1]
    for method, data in [('Successor', files[c][3])]:
        pre = tex_pre
        post = tex_post % (method, method, lambda_, x, p[c])

        in_ = tex_pre2 % (plain_size,)
        last_label = None
        for i, label in enumerate(labels):
            if i == 22 and method in ['Select', 'Successor']:
                continue
            if label != last_label:
                in_ += tex_post2
                in_ += tex_pre2 % (plain_size,)
            size = files[c][0][i][-2].split('=')[-1]
            latency = data[i][idx[method]]
            if int(latency) > 500:
                continue
            if size.endswith('m'):
                size = '0.00%s' % size.replace('.', '').replace('m', '')
            if float(size) / float(plain_size) > 1.4:
                continue
            in_ += ' ' * 16
            in_ += '\t'.join([label, size, latency])
            in_ += '\n'

            last_label = label
        in_ += tex_post2

        print(pre)
        print(in_)
        print(post)

