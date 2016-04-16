\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{mySlides}

% --------------------------------------------------------------
%                       Base class
% --------------------------------------------------------------

\LoadClass{beamer}
% \LoadClass[11pt]{article}


% --------------------------------------------------------------
%                          Style
% --------------------------------------------------------------

% \usetheme{CambridgeUS}
\usetheme{Berkeley}
\usecolortheme{dolphin}

% \definecolor{myorange}{RGB}{255,193,150}
% \definecolor{myorange}{RGB}{231,165,85}
% \mode<presentation>
%  {
%  \usetheme{Berkeley}
%  \usecolortheme{dolphin}
%  \setbeamercolor*{palette secondary}{use=structure,fg=white,bg=myorange}
%  % \setbeamercolor*{palette tertiary}{use=structure,fg=white,bg=green}
%  }

\beamertemplatenavigationsymbolsempty


% --------------------------------------------------------------
%                         List Style
% --------------------------------------------------------------

\setbeamertemplate{itemize items}[circle]
\setbeamertemplate{enumerate items}[circle]


% --------------------------------------------------------------
%                     Table of Contents
% --------------------------------------------------------------

\setbeamertemplate{section in toc}[circle]
% \setbeamertemplate{subsection in toc}[circle]
% \AtBeginSubsection[]
% {
%   \begin{frame}<beamer>
%     \frametitle{Outline}
%     \tableofcontents[currentsection,currentsubsection]
%   \end{frame}
% }
\AtBeginSection[]
{
  \begin{frame}<beamer>
    \frametitle{Outline}
    \tableofcontents[
        sectionstyle=show/shaded,
        subsectionstyle=show/show/shaded
    ]
  \end{frame}
}


% --------------------------------------------------------------
%                         Environments
% --------------------------------------------------------------

% \definecolor{fuchsia}{HTML}{FF0080}
\definecolor{fuchsia}{RGB}{121,28,123}
% \newenvironment<>{algo}[1][]{%
%   \setbeamercolor{block title}{fg=white,bg=fuchsia!75!black}%
%   \begin{block}{Algorithm (#1)}}{\end{block}}
\newenvironment<>{algo}[1][]{%
  \setbeamercolor{block title}{fg=white,bg=fuchsia!75!black}%
  \begin{block}{Algorithm: #1}}{\end{block}}



% --------------------------------------------------------------
%                            End
% --------------------------------------------------------------

\endinput
